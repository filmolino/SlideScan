#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mdns.h"

#include "project.h"
#include "config.h"
#include "wifi.h"

static const char *TAG = "WiFi";

static int retry_num = 0;
static bool sta_connected = false;
static bool sta_fail = false;
static bool ap_connected = false;
static config_wifi_t *cfg;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  ESP_LOGI(TAG, "wifi_event_handler event_id=%u", event_id);

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    if (cfg->sta) {
      ESP_LOGI(TAG, "WiFi STA_START trying to connect to network...");
      esp_wifi_connect();
    } else {
      ESP_LOGI(TAG, "WiFi connect as STA to AP is disabled!");
    }
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (retry_num < WIFI_STA_MAXIMUM_RETRY) {
      esp_wifi_connect();
      retry_num++;
      ESP_LOGI(TAG, "retry to connect to the AP");
    } else {
      sta_fail = true;
    }
    ESP_LOGI(TAG,"connect to the AP fail");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    retry_num = 0;
    sta_connected = true;
  }

  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    ap_connected = true;
    ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    ap_connected = false;
    ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
  }

  if (event_id == WIFI_EVENT_SCAN_DONE) {
    ESP_LOGI(TAG, "WIFI_EVENT_SCAN_DONE!!!");
  }
}

void wifi_init_ap_sta() {
  ESP_LOGI(TAG, "ESP_WIFI_MODE_AP_STA");

  wifi_config_t ap_config;
  wifi_config_t sta_config;

  cfg = config_get_wifi();      // Get configuration data

  // Config AP
  memset(&ap_config, 0, sizeof(wifi_config_t));
  strcpy((char*) &(ap_config.ap.ssid), cfg->ap_ssid);
  strcpy((char*) &(ap_config.ap.password), cfg->ap_password);
  ap_config.ap.channel = WIFI_CHANNEL;
  ap_config.ap.max_connection = WIFI_STA_MAX_CONN;
  ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;

  // Config STA
  memset(&sta_config, 0, sizeof(wifi_config_t));
  strcpy((char*) &(sta_config.sta.ssid), cfg->sta_ssid);
  strcpy((char*) &(sta_config.sta.password), cfg->sta_password);
  strcpy((char*) &(sta_config.sta.ssid), cfg->sta_ssid);
  strcpy((char*) &(sta_config.sta.password), cfg->sta_password);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "wifi_init_ap_sta finished. SSID:%s password:%s", ap_config.ap.ssid, ap_config.ap.password);


// SHOULD WE WAIT??
/*
  if (bits & WIFI_STA_CONNECTED_BIT) {
      ESP_LOGI(TAG, "connected to ap SSID:%s", cfg->sta_ssid);
  } else if (bits & WIFI_STA_FAIL_BIT) {
      ESP_LOGI(TAG, "Failed to connect to SSID:%s", cfg->sta_ssid);
  } else {
      ESP_LOGE(TAG, "UNEXPECTED EVENT");
  }
*/
}


void wifi_scan_start() {
  static uint16_t number = WIFI_SCAN_MAX_AP;
  static wifi_ap_record_t ap_info[WIFI_SCAN_MAX_AP];
  static uint16_t ap_count = 0;

  ESP_LOGI(TAG, "wifi_Scan_start...");
  ESP_LOGI(TAG, "size of ap_record is = %u", sizeof(wifi_ap_record_t));

  ESP_ERROR_CHECK(esp_wifi_disconnect());
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
  ESP_LOGI(TAG, "Scan DONE. ap_count=%u", ap_count);
  for (uint8_t n = 0; n < ap_count; n++) {
    ESP_LOGI(TAG, "ap_count[%u] SSID : %s", n, ap_info[n].ssid);
  }

  esp_wifi_scan_stop();
//  esp_wifi_connect();
}


bool wifi_get_state() {
  return (sta_connected || ap_connected) ? true :  false;
}


wifi_mode_t wifi_get_mode(){
  wifi_mode_t *mode = 0;
  esp_wifi_get_mode(mode);
  return *mode;
}


void wifi_init(void)
{
  ESP_LOGI(TAG, "Init");
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(mdns_init());
  ESP_ERROR_CHECK(mdns_hostname_set("slidescan"));
  ESP_LOGI(TAG, "mdns hostname set to: [slidescan]");

  esp_netif_create_default_wifi_sta();
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));             // No power save mode, faster reponse but higher power consumption
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));    // Save wifi config in RAM only
  
  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));
}
