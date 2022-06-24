#ifndef __config_h_
#define __config_h_

#include "ir_release.h"
#include "cable_release.h"
#include "config.h"

#define CONFIG_SCANNER_FILE_NAME                  "config.cfg"

#define CONFIG_SCANNER_AP_SSID                    "scanner"
#define CONFIG_SCANNER_AP_PASSWORD                "filmolino"

#define CONFIG_SCANNER_STA_SSID                   " "
#define CONFIG_SCANNER_STA_PASSWORD               " "


#define CONFIG_DEFAULT_SLIDE_CHANGE_TIME          2000      /* 2s time to change the slide */
#define CONFIG_DEFAULT_SLIDE_CHANGE_PULSE_TIME    250       /* 0.25s pulse time for slide change */
#define CONFIG_DEFAULT_SLIDE_SCAN_TIME            1000      /* 1s time to scan/capture the slide */
#define CONFIG_DEFAULT_SLIDES_TO_SCAN             50        /* Slides to scan (slides in magazine) */

#define CONFIG_DEFAULT_IR_ENABLED                 true     /* IR release active */
#define CONFIG_DEFAULT_IR_SELECTED_CODE           2         /* Selected IR release code [1..32] */
#define CONFIG_DEFAULT_CABLE_ENABLED              true      /* Cable release active */
#define CONFIG_DEFAULT_CABLE_SELECTED_CODE        1         /* Selected cable release code [1..32] */


typedef struct {
  char ap_ssid[32];                   /* Access Point Settings */
  char ap_password[64];
  char sta_ssid[32];                  /* Station Settings */
  char sta_password[64];
  bool ap;                            /* true if access point is active */
  bool sta;                           /* true if station mode is active */
} config_wifi_t;

typedef struct {
  uint32_t delay_slide_change;        /* Delay in ms */
  uint32_t time_slide_change_pulse;   /* pulse time in ms */
  uint32_t delay_slide_scan;          /* Delay in ms */
  uint8_t slides_to_scan;             /* Slides to scan */
  bool ir;                            /* true if active */
  uint8_t ir_selected_code;
  bool cable;                         /* true if active */
  uint8_t cable_selected_code;
} config_control_t;

typedef struct {
  ir_release_seq_t ir_codes[IR_RELEASE_CODES_SIZE];
  cable_release_seq_t cable_codes[CABLE_RELEASE_CODES_SIZE];
} config_release_t;

typedef struct {
  config_wifi_t     wifi;
  config_control_t  control;
  config_release_t  release;
} config_scanner_t;


// Exported functions
esp_err_t config_read();
esp_err_t config_save();
esp_err_t config_delete();
void config_defaults();
config_wifi_t *config_get_wifi();
uint32_t config_get_delay_slide_change();
uint32_t config_get_delay_slide_scan();
uint8_t config_get_slides_to_scan();
uint32_t config_get_time_slide_change_pulse();
bool config_get_ir_active();
bool config_get_cable_active();
uint8_t config_get_ir_divider(uint8_t code);
uint32_t config_get_ir_freq(uint8_t code);
uint8_t config_get_ir_selected_code();
rmt_item32_t *config_get_ir_seq();
void config_get_csv_cfg_wifi(char *csv);
void config_set_csv_cfg_wifi(char *csv);
void config_get_csv_cfg_scanner(char *csv);
void config_set_csv_cfg_scanner(char *csv);
void config_get_csv_cable_release_names(char *csv);
void config_get_csv_ir_release_names(char *csv);
void config_set_cable_release();
esp_err_t config_init();

#endif
