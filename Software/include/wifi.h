#ifndef __wifi_h_
#define __wifi_h_

#define WIFI_STA_MAXIMUM_RETRY    5 
#define WIFI_SCAN_MAX_AP          20

void wifi_init_ap_sta();
void wifi_scan_start();
bool wifi_get_state();
wifi_mode_t wifi_get_mode();
void wifi_init(void);

#endif