#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "project.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "config.h"
#include "control.h"
#include "cable_release.h"
#include "ir_release.h"
#include "wifi.h"
#include "server.h"

// LOG TAG
static const char *TAG = "app_main";

// Queues
static QueueHandle_t control_evt_queue = NULL;

// TASK: BUTTON 
void task_button(void *param) {
  static bool btnPressed[3] = { false, false, false };
  static TickType_t btnPressedTime[3] = { 0, 0 , 0 };
  gpio_num_t gpio = GPIO_BTN1;
  control_evt_t evt;

  while (1) {
    for (uint32_t n = 0; n < 3; n++) {
      if      (n == 0) { gpio = GPIO_BTN1; }
      else if (n == 1) { gpio = GPIO_BTN2; }
      else if (n == 2) { gpio = GPIO_INTERNAL_BUTTON; }
  
      if (gpio_get_level(gpio) == 0) {
        // Button is pressed
        if (!btnPressed[n]) {
          btnPressedTime[n] = 0;
          btnPressed[n] = true;
          ESP_LOGI(TAG, "button pressed[%d]", n);
        }
        else {
          btnPressedTime[n]++;
        }
      }
      else {
        // Button is not pressed
        if (btnPressed[n]) {
          // Button released => set Event
          btnPressed[n] = false;
          if (n == 0) { 
            evt = btnPressedTime[n] < BTN_SHORT_PRESS_TIME_MAX ? CONTROL_EVT_NEXT : CONTROL_EVT_LAST; 
          }
          if (n == 1) { 
            if (btnPressedTime[n] < BTN_SHORT_PRESS_TIME_MAX) {
              if (control_is_scanning()) {
                evt = CONTROL_EVT_STOP;
              } else {
                evt = CONTROL_EVT_SCAN;
              }
            } else { 
              control_reset_current_slide();    // Set current_slide to 0, so the scan starts with Image1
              evt = CONTROL_EVT_START; 
            }
          }
          if (n == 2) { 
            evt = btnPressedTime[n] < BTN_SHORT_PRESS_TIME_MAX ? CONTROL_EVT_BTN_INTERNAL_SHORT : CONTROL_EVT_BTN_INTERNAL_LONG; 
            wifi_scan_start();
          }
          ESP_LOGI(TAG, "button[%d] released, pressedTime=%d", n, btnPressedTime[n]);
          xQueueSend(control_evt_queue, (void *) &evt, 10);
        }
      }
    }
    vTaskDelay(BTN_POLL_PERIOD / portTICK_PERIOD_MS);
  }
}

// INIT HW
void init_hw() {

  // Button GPIO Init
  gpio_config_t config = {};
  config.mode = GPIO_MODE_INPUT;
  config.pull_up_en = GPIO_PULLUP_ENABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  config.pin_bit_mask = 1LL << GPIO_BTN1;
  gpio_config(&config);
  config.pin_bit_mask = 1LL << GPIO_BTN2;
  gpio_config(&config);
  config.pin_bit_mask = 1LL << GPIO_INTERNAL_BUTTON;
  gpio_config(&config);
}


esp_err_t init_spiffs() {
  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = false,
  };

  esp_err_t ret = esp_vfs_spiffs_register(&conf);
  if (ret == ESP_OK) {
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret == ESP_OK) {
      ESP_LOGI(TAG, "SPIFFS initialized. Partition size: total: %d, used: %d", total, used);
    } 
  } else {
    ESP_LOGI(TAG, "SPIFFS error %d", ret);
  }
  return ret;
}

void app_main() {
  esp_err_t ret;

  // HW Init
  init_hw();

  // Control
  ESP_LOGI(TAG, "----Init CONTROL...");
  control_init(GPIO_RELAY_LAST, GPIO_RELAY_NEXT, GPIO_LED_GREEN, GPIO_LED_RED);
  control_evt_queue = control_get_queue();

  //Initialize NVS
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // File System Init
  ESP_LOGI(TAG, "----Init SPIFFS...");
  init_spiffs();
  
  // CONFIG
  ESP_LOGI(TAG, "----Init CONFIG...");
  ret = config_init();
  if ( ret == ESP_OK) {
    // Cable & IR release
    uint8_t code = config_get_ir_selected_code();
    code = code > 0 ? code - 1 : 0;
    uint8_t divider = config_get_ir_divider(code);
    uint32_t freq = config_get_ir_freq(code);
    ir_release_init(GPIO_IR_RELEASE, RMT_CHANNEL_0, divider, freq);
    cable_release_init(GPIO_CABLE_RELEASE_SHUTTER, GPIO_CABLE_RELEASE_FOCUS);
    control_set_cable_release_queue();

    // Task button
    ESP_LOGI(TAG, "----Init TASK button...");
    xTaskCreate(&task_button, "task_button", 2048, NULL, 10, NULL);

    // WiFi
    ESP_LOGI(TAG, "----Init WiFi...");
    wifi_init();
    wifi_init_ap_sta();

    // http Server 
    server_init();
  } else {
    // Init Error
    control_evt_t evt;
    ESP_LOGE(TAG, "CONFIG ERROR err no = %d", ret);
    evt = CONTROL_EVT_ERROR; 
    xQueueSend(control_evt_queue, (void *) &evt, 10);
  }
}
