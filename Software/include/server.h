#ifndef __server_h_
#define __server_h_

#define SERVER_PATH_ROOT                "/"
#define SERVER_PATH_INDEX               "/index.html"

#define SERVER_WS_OK                    "OK,"
#define SERVER_WS_ERROR                 "ERROR,"

esp_err_t server_trigger_async_send(char *payload);
void server_init();

#endif