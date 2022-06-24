#include <string.h>
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "project.h"
#include "ir_release.h"
#include "ir_release_codes.h"
#include "cable_release.h"
#include "cable_release_codes.h"
#include "config.h"

// LOG TAG
static const char *TAG = "config";

static config_scanner_t config;
static char config_file_name[64];

// Read config from file returns ESP_OK if successful
esp_err_t config_read() {
  FILE *file = fopen(config_file_name, "rb");
  if (file == NULL) {
      return ESP_ERR_NOT_FOUND;
  }
  if (fread(&config, 1, sizeof(config_scanner_t), file) != sizeof(config_scanner_t)) {
      return ESP_ERR_INVALID_SIZE;
  }
  fclose(file);
  return ESP_OK;
}


// Save config file on SPIFFs and returns ESP_OK if successful
esp_err_t config_save() {
  FILE *file = fopen(config_file_name, "w");
  if (file == NULL) {
      return ESP_ERR_NOT_FOUND;
  }
  if (fwrite(&config, 1, sizeof(config_scanner_t), file) != sizeof(config_scanner_t)) {
    return ESP_ERR_INVALID_SIZE;
  }
  fclose(file);
  return ESP_OK;
}


// Delete config.cfg file on SPIFFS
esp_err_t config_delete() {
  int ret = remove(config_file_name);
  if (ret == 0) {
    return ESP_OK;
  }
  else {
    return ESP_ERR_NOT_FOUND;
  }
}


// Set defaults (doesn't save to SPIFFS)
void config_defaults() {
  uint8_t n;

  // WiFi settings
  strlcpy(config.wifi.ap_ssid, CONFIG_SCANNER_AP_SSID, sizeof(config.wifi.ap_ssid));
  strlcpy(config.wifi.ap_password, CONFIG_SCANNER_AP_PASSWORD, sizeof(config.wifi.ap_password));
  strlcpy(config.wifi.sta_ssid, CONFIG_SCANNER_STA_SSID, sizeof(config.wifi.sta_ssid));
  strlcpy(config.wifi.sta_password, CONFIG_SCANNER_STA_PASSWORD, sizeof(config.wifi.sta_password));
  config.wifi.ap = true;    // Access point is active
  config.wifi.sta = false;    // Station Mode is not active
  // Control settings
  config.control.delay_slide_change = CONFIG_DEFAULT_SLIDE_CHANGE_TIME;
  config.control.time_slide_change_pulse = CONFIG_DEFAULT_SLIDE_CHANGE_PULSE_TIME;
  config.control.delay_slide_scan = CONFIG_DEFAULT_SLIDE_SCAN_TIME;
  config.control.slides_to_scan = CONFIG_DEFAULT_SLIDES_TO_SCAN;
  config.control.ir = CONFIG_DEFAULT_IR_ENABLED;
  config.control.ir_selected_code = CONFIG_DEFAULT_IR_SELECTED_CODE;
  config.control.cable = CONFIG_DEFAULT_CABLE_ENABLED;
  config.control.cable_selected_code = CONFIG_DEFAULT_CABLE_SELECTED_CODE;
  // IR Release settings
  memcpy(&config.release.ir_codes[0].ir_seq[0].val, ir_release_codes_nikon, sizeof(ir_release_codes_nikon));            // Nikon
  config.release.ir_codes[0].freq = ir_release_codes_nikon_freq;
  config.release.ir_codes[0].divider = ir_release_codes_nikon_divider;
  config.release.ir_codes[0].name = (char*) ir_release_codes_nikon_name;
  memcpy(&config.release.ir_codes[1].ir_seq[0].val, ir_release_codes_pentax, sizeof(ir_release_codes_pentax));          // Pentax
  config.release.ir_codes[1].freq = ir_release_codes_pentax_freq;
  config.release.ir_codes[1].divider = ir_release_codes_pentax_divider;
  config.release.ir_codes[1].name = (char*) ir_release_codes_pentax_name;
  memcpy(&config.release.ir_codes[2].ir_seq[0].val, ir_release_codes_olympus, sizeof(ir_release_codes_olympus));        // Olympus
  config.release.ir_codes[2].freq = ir_release_codes_olympus_freq;
  config.release.ir_codes[2].divider = ir_release_codes_olympus_divider;
  config.release.ir_codes[2].name = (char*) ir_release_codes_olympus_name;
  memcpy(&config.release.ir_codes[3].ir_seq[0].val, ir_release_codes_minolta, sizeof(ir_release_codes_minolta));        // Minolta
  config.release.ir_codes[3].freq = ir_release_codes_minolta_freq;
  config.release.ir_codes[3].divider = ir_release_codes_minolta_divider;
  config.release.ir_codes[3].name = (char*) ir_release_codes_minolta_name;
  memcpy(&config.release.ir_codes[4].ir_seq[0].val, ir_release_codes_sony, sizeof(ir_release_codes_sony));              // Sony
  config.release.ir_codes[4].freq = ir_release_codes_sony_freq;
  config.release.ir_codes[4].divider = ir_release_codes_sony_divider;
  config.release.ir_codes[4].name = (char*) ir_release_codes_sony_name;
  memcpy(&config.release.ir_codes[5].ir_seq[0].val, ir_release_codes_canon, sizeof(ir_release_codes_canon));            // Canon
  config.release.ir_codes[5].freq = ir_release_codes_canon_freq;
  config.release.ir_codes[5].divider = ir_release_codes_canon_divider;
  config.release.ir_codes[5].name = (char*) ir_release_codes_canon_name;

  for (n = 6; n < IR_RELEASE_CODES_SIZE; n++) {
    memcpy(&config.release.ir_codes[5].ir_seq[0].val, ir_release_codes_default, sizeof(ir_release_codes_default));
    config.release.ir_codes[n].freq = ir_release_codes_default_freq;
    config.release.ir_codes[n].divider = ir_release_codes_default_divider;
    config.release.ir_codes[n].name = (char*) ir_release_codes_default_name;
  }

  // Cable Release settings
  memcpy(&config.release.cable_codes[0].cable_seq[0], cable_release_codes_basic, sizeof(cable_release_codes_basic));    // Basic
  config.release.cable_codes[0].name = (char*) cable_release_codes_basic_name;

  for (n = 1; n < CABLE_RELEASE_CODES_SIZE; n++) {
    memcpy(&config.release.cable_codes[n].cable_seq[0], cable_release_codes_default, sizeof(cable_release_codes_default));    // Basic
    config.release.cable_codes[n].name = (char*) cable_release_codes_default_name;
  }
}

char* config_get_ap_ssid() {
  return config.wifi.ap_ssid;
}

config_wifi_t *config_get_wifi() {
  return &config.wifi;
}

uint32_t config_get_delay_slide_change() {
  return config.control.delay_slide_change;
}

uint32_t config_get_delay_slide_scan() {
  ESP_LOGI(TAG, "config.control.delay_slide_scan=%u", config.control.delay_slide_scan);
  return config.control.delay_slide_scan;
}

uint8_t config_get_slides_to_scan() {
  return config.control.slides_to_scan;
}

uint32_t config_get_time_slide_change_pulse() {
  return config.control.time_slide_change_pulse;
}

bool config_get_ir_active() {
  return config.control.ir;
}

bool config_get_cable_active() {
  return config.control.cable;
}

uint8_t config_get_ir_divider(uint8_t code) {
  return config.release.ir_codes[code].divider;
}

uint32_t config_get_ir_freq(uint8_t code) {
  return config.release.ir_codes[code].freq;
}

uint8_t config_get_ir_selected_code() {
  return config.control.ir_selected_code;
}

rmt_item32_t *config_get_ir_seq() {
  uint8_t code = config.control.ir_selected_code;      // Attention code is in the range from 01...IR_RELEASE_CODES_SIZE

  if ((code > 0) && (code < IR_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }
  return &config.release.ir_codes[code].ir_seq[0];
}


void config_get_csv_cfg_wifi(char *csv) {
  sprintf(csv, "WiFiCfg,%s,%s,%s,%s,%d,%d", 
          config.wifi.ap_ssid, 
          config.wifi.ap_password,
          config.wifi.sta_ssid, 
          config.wifi.sta_password,
          config.wifi.ap,
          config.wifi.sta);
}

void config_set_csv_cfg_wifi(char *csv) {
  char *data;

  data = strtok(csv, ",");    // CMD not used here
  data = strtok(NULL, ",");
  strcpy(config.wifi.ap_ssid, data);
  data = strtok(NULL, ",");
  strcpy(config.wifi.ap_password, data);
  data = strtok(NULL, ",");
  strcpy(config.wifi.sta_ssid, data);
  data = strtok(NULL, ",");
  strcpy(config.wifi.sta_password, data);
  data = strtok(NULL, ",");
  config.wifi.ap = (strcmp(data, "1") == 0) ? true : false;
  data = strtok(NULL, ",");
  config.wifi.sta = (strcmp(data, "1") == 0) ? true : false;

  ESP_LOGI(TAG, "ap_ssid = %s", config.wifi.ap_ssid);
  ESP_LOGI(TAG, "ap_ssid = %s", config.wifi.ap_password);
  ESP_LOGI(TAG, "sta_ssid = %s", config.wifi.sta_ssid);
  ESP_LOGI(TAG, "sta_password = %s", config.wifi.sta_password);
  ESP_LOGI(TAG, "ap = %u", config.wifi.ap);
  ESP_LOGI(TAG, "sta = %u", config.wifi.sta);
}

void config_get_csv_cfg_scanner(char *csv) {
  sprintf(csv, "ScannerCfg,%u,%u,%u,%u,%d,%u,%d,%u", 
          config.control.delay_slide_change,
          config.control.time_slide_change_pulse,
          config.control.delay_slide_scan, 
          config.control.slides_to_scan,
          config.control.ir,
          config.control.ir_selected_code,
          config.control.cable,
          config.control.cable_selected_code);
}

void config_set_csv_cfg_scanner(char *csv) {
  char *data;

  data = strtok(csv, ",");    // CMD not used here
  data = strtok(NULL, ",");
  config.control.delay_slide_change = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  config.control.time_slide_change_pulse = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  config.control.delay_slide_scan = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  config.control.slides_to_scan = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  config.control.ir = (strcmp(data, "1") == 0) ? true : false;
  data = strtok(NULL, ",");
  config.control.ir_selected_code = strtoul(data, NULL, 10);
  data = strtok(NULL, ",");
  config.control.cable = (strcmp(data, "1") == 0) ? true : false;
  data = strtok(NULL, ",");
  config.control.cable_selected_code = strtoul(data, NULL, 10);

  ESP_LOGI(TAG, "delay_slide_change = %u", config.control.delay_slide_change);
  ESP_LOGI(TAG, "time_slide_change_pulse = %u", config.control.time_slide_change_pulse);
  ESP_LOGI(TAG, "delay_slide_scan = %u", config.control.delay_slide_scan);
  ESP_LOGI(TAG, "slides_to_scan = %u", config.control.slides_to_scan);
  ESP_LOGI(TAG, "ir = %u", config.control.ir);
  ESP_LOGI(TAG, "ir_selected_code = %u", config.control.ir_selected_code);
  ESP_LOGI(TAG, "cable = %u", config.control.cable);
  ESP_LOGI(TAG, "cable_selected_code = %u", config.control.cable_selected_code);
}


void config_get_csv_cable_release_names(char *csv) {
  char *separator = ",";

  sprintf(csv, "getCableRelNames");
  for (uint8_t n = 0; n < CABLE_RELEASE_CODES_SIZE ; n++) {
    strcat(csv, separator);
    strcat(csv, config.release.cable_codes[n].name);
  }
}


void config_get_csv_ir_release_names(char *csv) {
  char *separator = ",";

  sprintf(csv, "getIrRelNames");
  for (uint8_t n = 0; n < IR_RELEASE_CODES_SIZE ; n++) {
    strcat(csv, separator);
    strcat(csv, config.release.ir_codes[n].name);
  }
}


void config_set_cable_release() {
  uint8_t code = config.control.cable_selected_code;      // Attention code is in the range from 01...CABLE_RELEASE_CODES_SIZE

  if ((code > 0) && (code < CABLE_RELEASE_CODES_SIZE)) {
    code--;   // Array starts at [0]
  } else {
    code = 0;
  }
  cable_release_set_seq(&config.release.cable_codes[code].cable_seq[0]);
}


esp_err_t config_init() {
  // Set path to the config file
  sprintf(config_file_name, "%s/%s", SPIFFS_PATH, CONFIG_SCANNER_FILE_NAME);

  // Load config file, if it not exists load defaults and save file
  esp_err_t ret = config_read();
  if (ret) {
    // file config.cfg doesn't exist, so generate it
    config_defaults();
    ret = config_save();
  }
  if (ret == ESP_OK) {
    config_set_cable_release();
  }
  return ret;
}
