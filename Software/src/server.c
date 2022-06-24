#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "control.h"
#include "config.h"
#include "wifi.h"
#include "server.h"

// Local types
typedef struct {
    uint8_t payload[32];
    httpd_handle_t hd;
    int fd;
} async_resp_arg_t;

// Local variables
static const char *TAG = "server";
async_resp_arg_t ws_async_arg;
bool ws_connected = false;

// Local functions
static char* http_content_type(char *path);
static esp_err_t webserver_read_file(httpd_req_t *req);
static void send_control_event(control_evt_t evt);
static esp_err_t server_find_networks(uint8_t *payload);

// Queues
static QueueHandle_t control_evt_queue = NULL;


// *********************************************************************************
// ROUTES
// *********************************************************************************
// ROOT
static esp_err_t root_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Request URI \"%s\"", req->uri);
    esp_err_t ret = webserver_read_file(req);
    httpd_resp_sendstr_chunk(req, NULL);
    return ret;
}

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
};

// WEBSOCKET
static esp_err_t web_socket_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        // Handles & descriptors used for ws async send
        ws_async_arg.hd = req->handle;
        ws_async_arg.fd = httpd_req_to_sockfd(req);
        ws_connected = true;
        return ESP_OK;
    }
    
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        buf = calloc(1, 1024);          // Big enough for scan results
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);
    
    // TEXT DATA
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
        if (strcmp((char*)ws_pkt.payload,"btnStartStop") == 0) {
            ESP_LOGI(TAG, "[WS] btnStartStop");
            if (control_is_scanning()) {
                send_control_event(CONTROL_EVT_STOP);
                sprintf((char*)ws_pkt.payload, "AutoScan,0");               // Send scanning state
            } else {
                send_control_event(CONTROL_EVT_START);
                sprintf((char*)ws_pkt.payload, "AutoScan,1");               // Send scanning state
            }
        } 
        else if (strcmp((char*)ws_pkt.payload,"btnLast") == 0) {
            ESP_LOGI(TAG, "[WS] btnLast");
            send_control_event(CONTROL_EVT_LAST);
            sprintf((char*)ws_pkt.payload, SERVER_WS_OK);               // Websocket Answer
        }
        else if (strcmp((char*)ws_pkt.payload,"btnNext") == 0) {
            ESP_LOGI(TAG, "[WS] btnNext");
            send_control_event(CONTROL_EVT_NEXT);
            sprintf((char*)ws_pkt.payload, SERVER_WS_OK);               // Websocket Answer
        }
        else if (strcmp((char*)ws_pkt.payload,"btnScan") == 0) {
            ESP_LOGI(TAG, "[WS] btnScan");
            send_control_event(CONTROL_EVT_SCAN);
            sprintf((char*)ws_pkt.payload, SERVER_WS_OK);               // Websocket Answer
        }
        else if (strcmp((char*)ws_pkt.payload,"getCurrentSlide") == 0) {
            ESP_LOGI(TAG, "[WS] getCurrentSlide");
            control_get_current_slide((char *) ws_pkt.payload);
        }
        else if (strncmp((char*)ws_pkt.payload,"setCurrentSlide", 15) == 0) {
            ESP_LOGI(TAG, "[WS] setCurrentSlide");
            send_control_event(CONTROL_EVT_RESET_CURRENT_SLIDES);
        }
        else if (strcmp((char*)ws_pkt.payload,"getWiFiCfg") == 0) {
            ESP_LOGI(TAG, "[WS] getWiFiCfg");
            config_get_csv_cfg_wifi((char *) ws_pkt.payload);
        }
        else if (strncmp((char*)ws_pkt.payload,"setWiFiCfg", 10) == 0) {
            ESP_LOGI(TAG, "[WS] setWiFiCfg");
            config_set_csv_cfg_wifi((char *) ws_pkt.payload);
            sprintf((char*)ws_pkt.payload, SERVER_WS_OK);               // Websocket Answer
        }
        else if (strcmp((char*)ws_pkt.payload,"getScanCfg") == 0) {
            ESP_LOGI(TAG, "[WS] getScanCfg");
            config_get_csv_cfg_scanner((char *) ws_pkt.payload);
        }
        else if (strncmp((char*)ws_pkt.payload,"setScanCfg", 10) == 0) {
            ESP_LOGI(TAG, "[WS] setScanCfg");
            config_set_csv_cfg_scanner((char *) ws_pkt.payload);
            sprintf((char*)ws_pkt.payload, SERVER_WS_OK);               // Websocket Answer
        }
        else if (strcmp((char*)ws_pkt.payload,"getCableRelNames") == 0) {
            ESP_LOGI(TAG, "[WS] getCableRelNames");
            config_get_csv_cable_release_names((char *) ws_pkt.payload);
            ESP_LOGI(TAG, "[WS] AFTER getCableRelNames");
        }
        else if (strcmp((char*)ws_pkt.payload,"getIrRelNames") == 0) {
            ESP_LOGI(TAG, "[WS] getIrRelNames");
            config_get_csv_ir_release_names((char *) ws_pkt.payload);
        }
        else if (strcmp((char*)ws_pkt.payload,"findNetworks") == 0) {
            ESP_LOGI(TAG, "[WS] findNetworks");
            if (server_find_networks(ws_pkt.payload) != ESP_OK) {
                sprintf((char*)ws_pkt.payload, SERVER_WS_ERROR);        // Websocket Answer                
            }
        } 
        else if (strcmp((char*)ws_pkt.payload,"restartSystem") == 0) {
            ESP_LOGI(TAG, "[WS] restartSystem");
            esp_restart();
        } 
        else if (strcmp((char*)ws_pkt.payload,"deleteConfig") == 0) {
            ESP_LOGI(TAG, "[WS] deleteConfig");
            config_delete();
            sprintf((char*)ws_pkt.payload, SERVER_WS_OK);               // Websocket Answer
        }
        else if (strcmp((char*)ws_pkt.payload,"saveConfig") == 0) {
            ESP_LOGI(TAG, "[WS] saveConfig");            
            config_save();
            sprintf((char*)ws_pkt.payload, SERVER_WS_OK);               // Websocket Answer
        }
        else {
            sprintf((char*)ws_pkt.payload, SERVER_WS_ERROR);            // Websocket Answer
        }
        // Send data
        ws_pkt.final = true;
        ws_pkt.fragmented = false;
        ws_pkt.len = strlen((char *) ws_pkt.payload);
        ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
    }
    // CLOSE
    else if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
        ws_connected = false;
        ESP_LOGI(TAG, "Websocket CLOSED");
    }
    free(buf);
    return ret;
}

static const httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = web_socket_handler,
        .user_ctx   = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true        /* Handle also Ping, Pong & Close */
};


// SCRIPT JS
static esp_err_t script_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Request URI \"%s\"", req->uri);
    esp_err_t ret = webserver_read_file(req);
    httpd_resp_sendstr_chunk(req, NULL);
    return ret;
}

static const httpd_uri_t script = {
    .uri       = "/script.js",
    .method    = HTTP_GET,
    .handler   = script_get_handler,
};


// PICO CSS
static esp_err_t pico_css_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Request URI \"%s\"", req->uri);
    esp_err_t ret = webserver_read_file(req);
    httpd_resp_sendstr_chunk(req, NULL);
    return ret;
}

static const httpd_uri_t pico_css = {
    .uri       = "/pico.min.css",
    .method    = HTTP_GET,
    .handler   = pico_css_get_handler,
};


// FAVICON.ICO
static esp_err_t favicon_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Request URI \"%s\"", req->uri);
    esp_err_t ret = webserver_read_file(req);
    httpd_resp_sendstr_chunk(req, NULL);
    return ret;
}

static const httpd_uri_t favicon = {
    .uri       = "/favicon.ico",
    .method    = HTTP_GET,
    .handler   = favicon_get_handler,
};


// *********************************************************************************
// HANDLERS for connect, disconnet
// *********************************************************************************
static void webserver_connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    // Start webserver
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 20;
    config.max_open_sockets = 13;
    esp_err_t ret = ESP_FAIL;

    ESP_LOGI(TAG, "http client connected, serving...");

    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        ret = httpd_register_uri_handler(server, &root);
        if (ret != ESP_OK) ESP_LOGE(TAG, "URL \"%s\" not registered", root.uri);
        ret = httpd_register_uri_handler(server, &ws);
        if (ret != ESP_OK) ESP_LOGE(TAG, "URL \"%s\" not registered", ws.uri);
        ret = httpd_register_uri_handler(server, &script);
        if (ret != ESP_OK) ESP_LOGE(TAG, "URL \"%s\" not registered", script.uri);
        ret = httpd_register_uri_handler(server, &favicon);
        if (ret != ESP_OK) ESP_LOGE(TAG, "URL \"%s\" not registered", favicon.uri);
        ret = httpd_register_uri_handler(server, &pico_css);
        if (ret != ESP_OK) ESP_LOGE(TAG, "URL \"%s\" not registered", pico_css.uri);
    }
}


static void webserver_disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    httpd_handle_t *server = (httpd_handle_t*) arg;
    ESP_LOGI(TAG, "Stopping webserver");
    httpd_stop(server);
}

static char* http_content_type(char *path) {
    char *ext = strrchr(path, '.');
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".tpl") == 0) return "text/html";
    if (strcmp(ext, ".svg") == 0) return "image/svg+xml";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "text/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".ico") == 0) return "image/x-icon";
    return "text/plain";
}


static esp_err_t webserver_read_file(httpd_req_t *req) {
    char buff[1024 + 1];
    size_t size;

    if (strcmp(req->uri, SERVER_PATH_ROOT) == 0) {
        strcpy((char*) req->uri, SERVER_PATH_INDEX);
    }
    sprintf(buff, "/spiffs%s", req->uri);
    ESP_LOGI(TAG, "spiffs path: %s", buff);
    
    ESP_LOGI(TAG, "type: %s", http_content_type(buff));
    httpd_resp_set_type(req, http_content_type(buff));    

    FILE *file = fopen(buff, "rb");
    if (file == NULL) {
        ESP_LOGE(TAG, "Cannot open file /spiffs/index.html");
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not Found");
        return ESP_FAIL;
    }
    
    while ((size = fread(buff, 1, sizeof(buff) - 1, file)) > 0) {
        httpd_resp_send_chunk(req, buff, size);
    }

    ESP_LOGI(TAG, "END! fclose");
    fclose(file);

    return ESP_OK;
}


static void send_control_event(control_evt_t evt) {
    xQueueSend(control_evt_queue, (void *) &evt, 10);
}


static esp_err_t server_find_networks(uint8_t *payload) {
    esp_err_t ret;

    ESP_LOGI(TAG, "Start scanning...");
    ret = esp_wifi_scan_start(NULL, true);

    if (ret == ESP_OK) {
        wifi_ap_record_t ap_records[WIFI_SCAN_MAX_AP];
        uint16_t ap_num;
        char ap_entry[64];

        ESP_LOGI(TAG, "DONE!");
        esp_wifi_scan_get_ap_num(&ap_num);
        ap_num = WIFI_SCAN_MAX_AP;
        esp_wifi_scan_get_ap_records(&ap_num, &ap_records[0]);

        sprintf((char *) payload, "NetworksFound,%u", ap_num);
        for (uint8_t n = 0; (n < ap_num) && (n < WIFI_SCAN_MAX_AP); n++) {
            ESP_LOGI(TAG, "AP ssid:%s  |  rssi:%u", ap_records[n].ssid, ap_records[n].rssi);
            sprintf(ap_entry, ",%s,%u", ap_records[n].ssid, ap_records[n].rssi);
            strcat((char *) payload, ap_entry);
        }
    }
    return ret;
}

static void server_send_async_data(void *payload) {
    httpd_handle_t hd = ws_async_arg.hd;
    int fd = ws_async_arg.fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

    ws_pkt.payload = (uint8_t *) payload;
    ws_pkt.len = strlen(payload);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
}

esp_err_t server_trigger_async_send(char *payload) {
    if (ws_connected) {
        memset(ws_async_arg.payload, 0, sizeof(ws_async_arg.payload));
        strncpy((char *) ws_async_arg.payload, payload, sizeof(ws_async_arg.payload) - 1);         // -1 for EOS char
        return httpd_queue_work(ws_async_arg.hd, server_send_async_data, ws_async_arg.payload);
    }
    else {
        return ESP_OK;
    }
}



void server_init() {
    // Register handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &webserver_connect_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &webserver_connect_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &webserver_disconnect_handler, NULL, NULL));
  
  control_evt_queue = control_get_queue();
}