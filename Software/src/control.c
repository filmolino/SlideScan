#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "project.h"
#include "config.h"
#include "server.h"
#include "wifi.h"
#include "control.h"

// Local Types
typedef enum {
  LED_STATE_OFF,
  LED_STATE_ON,
  LED_STATE_BLINK,
  LED_STATE_BLINK_SLOW,
} led_states_t;

// LOG TAG
static const char *TAG = "control";

// Local variables
static QueueHandle_t control_evt_queue = NULL;
static QueueHandle_t cable_release_evt_queue = NULL;
static control_states_t _current_state;
static bool autoScan = false;
static uint8_t current_slide = 0;
static gpio_num_t pin_last = 0;
static gpio_num_t pin_next = 0;
static gpio_num_t pin_led_gn = 0;
static gpio_num_t pin_led_rd = 0;
static esp_timer_handle_t delay_timer;
static esp_timer_handle_t pulse_timer;

// Local functions
static void state_idle_entry(void);
static void state_error_entry(void);
static void state_next_entry(void);
static void state_last_entry(void);
static void state_scan_entry(void);
static void control_start_delay_timer(uint32_t delay_ms);
static void control_stop_delay_timer();
static void control_start_pulse_timer(uint32_t delay_ms);
static void control_stop_pulse_timer();
static void control_update_current_slide(uint8_t val);
static void control_update_auto_scan_button();


// Timer Handler
static void delay_timer_callback(void* arg)
{
  ESP_LOGI(TAG, "delay_timer Handler");

  control_evt_t evt = CONTROL_EVT_TIMER;
  xQueueSend(control_evt_queue, (void *) &evt, 10);
}

static void pulse_timer_callback(void* arg)
{
  ESP_LOGI(TAG, "pulse_timer Handler");

  gpio_set_level(pin_last, 0);
  gpio_set_level(pin_next, 0);

  if (!autoScan) {
    control_evt_t evt = CONTROL_EVT_TIMER;
    xQueueSend(control_evt_queue, (void *) &evt, 10);
  }
}


// --------------------------------------------------------------------------------------------------
// State: IDLE
// --------------------------------------------------------------------------------------------------
static void state_idle_entry(void) {
  ESP_LOGI(TAG, "entry: CONTROL_STATE_IDLE");
  _current_state = CONTROL_STATE_IDLE;
  gpio_set_level(pin_last, 0);
  gpio_set_level(pin_next, 0);
  control_stop_delay_timer();
  control_stop_pulse_timer();
  autoScan = false;
}

static void state_idle(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      autoScan = false;
      state_next_entry();
      break;

    case CONTROL_EVT_LAST:
      autoScan = false;
      state_last_entry();
      break;

    case CONTROL_EVT_START:
      autoScan = true; 
      state_next_entry();
      break;

    case CONTROL_EVT_STOP:
      break;

    case CONTROL_EVT_SCAN:
      autoScan = false;
      state_scan_entry();
      break;

    case CONTROL_EVT_TIMER:
      break;

    case CONTROL_EVT_RESET_CURRENT_SLIDES:
      control_update_current_slide(0);
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, stay in IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: ERROR
// --------------------------------------------------------------------------------------------------
static void state_error_entry(void) {
  ESP_LOGI(TAG, "entry: CONTROL_STATE_ERROR");
  _current_state = CONTROL_STATE_ERROR;
}

static void state_error(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER:
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, stay in IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: CONTROL_STATE_NEXT
// --------------------------------------------------------------------------------------------------
static void state_next_entry(void) {
  ESP_LOGI(TAG, "entry: CONTROL_STATE_NEXT");
  if (current_slide < config_get_slides_to_scan()) {
    control_update_current_slide(current_slide + 1);    
    _current_state = CONTROL_STATE_NEXT;
    gpio_set_level(pin_next, 1);
    control_start_delay_timer(config_get_delay_slide_change());
    control_start_pulse_timer(config_get_time_slide_change_pulse());
  }
  else {
    // all slides are scanned, nothing to do
    state_idle_entry();
  }
}

static void state_next(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      state_idle_entry();
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER:
      autoScan ? state_scan_entry() : state_idle_entry();
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, go to IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: CONTROL_STATE_LAST
// --------------------------------------------------------------------------------------------------
static void state_last_entry(void) {
  ESP_LOGI(TAG, "entry: CONTROL_STATE_LAST");
  if (current_slide > 0) {
    control_update_current_slide(current_slide - 1);
    _current_state = CONTROL_STATE_LAST;
    gpio_set_level(pin_last, 1);
    control_start_delay_timer(config_get_delay_slide_change());
    control_start_pulse_timer(config_get_time_slide_change_pulse());  
  }
  else {
    // First slide reached, nothing to scan
    state_idle_entry();
  }
}

static void state_last(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      state_idle_entry();
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER:
      state_idle_entry();
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, go to IDLE state
      break;
  }
}


// --------------------------------------------------------------------------------------------------
// State: SCAN
// --------------------------------------------------------------------------------------------------
static void state_scan_entry(void) {
  ESP_LOGI(TAG, "entry: CONTROL_STATE_SCAN");
  _current_state = CONTROL_STATE_SCAN;

  if (config_get_ir_active()) {
    ESP_LOGI(TAG, "IR send");
    ir_release_send(config_get_ir_seq());
  }
  
  if (config_get_cable_active()) {
    ESP_LOGI(TAG, "CABLE send");
    cable_release_evt_t evt = CABLE_RELEASE_EVT_START;
    xQueueSend(cable_release_evt_queue, (void *) &evt, 10);
  }

  control_start_delay_timer(config_get_delay_slide_scan());
}

static void state_scan(control_evt_t argEvt) {
  switch (argEvt) {
    case CONTROL_EVT_NONE:
      break;

    case CONTROL_EVT_NEXT:
      break;

    case CONTROL_EVT_LAST:
      break;

    case CONTROL_EVT_START:
      break;

    case CONTROL_EVT_STOP:
      state_idle_entry();
      break;

    case CONTROL_EVT_SCAN:
      break;

    case CONTROL_EVT_TIMER:
      if ((current_slide < config_get_slides_to_scan()) && autoScan) {
        state_next_entry();
      } else {
        control_update_auto_scan_button();
        state_idle_entry();
      }
      break;

    case CONTROL_EVT_ERROR:
      state_error_entry();
      break;

    default:
      state_idle_entry();   // Should not happen, go to IDLE state
      break;
  }
}


// Statemachine
static void control_sm(control_evt_t argEvt) 
{
  ESP_LOGI(TAG, "control_sm current_state=%u  |  argEvt=%u", _current_state, argEvt);

  switch (_current_state) {
    case CONTROL_STATE_IDLE:        state_idle(argEvt); break;
    case CONTROL_STATE_ERROR:       state_error(argEvt); break;
    case CONTROL_STATE_NEXT:        state_next(argEvt); break;
    case CONTROL_STATE_LAST:        state_last(argEvt); break;
    case CONTROL_STATE_SCAN:        state_scan(argEvt); break;
    default:                        state_idle_entry(); break;
  }
}

// TIMER delay
static void control_start_delay_timer(uint32_t delay_ms) {
  ESP_LOGI(TAG, "Starting delay_timer delay=%u", delay_ms * 1000);
  esp_timer_stop(delay_timer);
  esp_timer_start_once(delay_timer, delay_ms * 1000);
}

static void control_stop_delay_timer() {
  esp_timer_stop(delay_timer);
}


// TIMER pulse
static void control_start_pulse_timer(uint32_t delay_ms) {
  esp_timer_stop(pulse_timer);
  esp_timer_start_once(pulse_timer, delay_ms * 1000);
}

static void control_stop_pulse_timer() {
  esp_timer_stop(pulse_timer);
}


void control_set_current_slide(char *payload) {
  char *data;
  uint8_t slide;

  data = strtok(payload, ",");    // CMD not used here
  data = strtok(NULL, ",");
  slide = strtoul(data, NULL, 10);
  control_update_current_slide(slide);
}

void control_reset_current_slide() {
  current_slide = 0;
  ESP_LOGI(TAG, "current_slide = %u", current_slide);
}


void control_get_current_slide(char *payload) {
  sprintf(payload, "CurrentSlide,%u", current_slide);
}


static void control_update_current_slide(uint8_t val) {
  char data[32];

  current_slide = val;
  sprintf(data, "CurrentSlide,%u", current_slide);
  server_trigger_async_send(data);
  ESP_LOGI(TAG, "current_slide = %u", current_slide);
}


// Changes button color to inactive
static void control_update_auto_scan_button() {
  char data[32];

  sprintf(data, "AutoScan,0");
  server_trigger_async_send(data);
}


// Control Task
void control_task(void *param) {
  control_evt_t evt;

  ESP_LOGI(TAG, "control_task created");

  while (1) {
    if (xQueueReceive(control_evt_queue, (void*) &evt, 10)) {      
      control_sm(evt);
    }
  }
}


// LED Task
void control_led_task(void *param) {
  ESP_LOGI(TAG, "control_led_task created");

  static led_states_t led_gn;
  static led_states_t led_rd;
  static uint8_t slow_blink = 0;
  static bool led_blink_state = false;
  static bool led_blink_slow_state = false;

  while (1) {
    if (_current_state == CONTROL_STATE_ERROR) {
      // Error others
      led_gn = LED_STATE_OFF;
      led_rd = LED_STATE_ON;
    } else if (autoScan) {
      // Auto Scan active
      led_gn = LED_STATE_BLINK;
      led_rd = LED_STATE_OFF;
    } else if (!wifi_get_state()) {
      // WiFi settings error (manual mode still works)
      led_gn = LED_STATE_ON;
      led_rd = LED_STATE_BLINK_SLOW;
    } else {
      // OK
      led_gn = LED_STATE_ON;
      led_rd = LED_STATE_OFF;
    }

    switch (led_gn) {
      case LED_STATE_OFF:         gpio_set_level(pin_led_gn, 0); break;
      case LED_STATE_ON:          gpio_set_level(pin_led_gn, 1); break;
      case LED_STATE_BLINK:       led_blink_state ? gpio_set_level(pin_led_gn, 1) : gpio_set_level(pin_led_gn, 0); break;
      case LED_STATE_BLINK_SLOW:  led_blink_slow_state ? gpio_set_level(pin_led_gn, 1) : gpio_set_level(pin_led_gn, 0); break;
    }

    switch (led_rd) {
      case LED_STATE_OFF:         gpio_set_level(pin_led_rd, 0); break;
      case LED_STATE_ON:          gpio_set_level(pin_led_rd, 1); break;
      case LED_STATE_BLINK:       led_blink_state ? gpio_set_level(pin_led_rd, 1) : gpio_set_level(pin_led_rd, 0); break;
      case LED_STATE_BLINK_SLOW:  led_blink_slow_state ? gpio_set_level(pin_led_rd, 1) : gpio_set_level(pin_led_rd, 0); break;
    }
  
    led_blink_state ^= 1;   // Toggle

    if (slow_blink >= LED_BLINK_SLOW_FACTOR) {
      slow_blink = 0;
      led_blink_slow_state = true;
    } else { 
      slow_blink++; 
      led_blink_slow_state = false;
    } 

    vTaskDelay(LED_BLINK_PERIOD);
  }
}

// Get ptr to event queue
QueueHandle_t control_get_queue() {
  return control_evt_queue;
}

// Set cable release queue
void control_set_cable_release_queue() {
  cable_release_evt_queue = cable_release_get_queue_ptr();
  ESP_LOGI(TAG, "cable_release_evt_queue %u", cable_release_evt_queue);
}

// Get scan state
bool control_is_scanning() {
  return autoScan;
}

// INIT
void control_init(gpio_num_t last, gpio_num_t next, gpio_num_t led_gn, gpio_num_t led_rd) {
  _current_state = CONTROL_STATE_IDLE;

  // GPIO
  pin_last = last;
  pin_next = next;
  pin_led_gn = led_gn;
  pin_led_rd = led_rd;
  gpio_config_t config = {};
  config.mode = GPIO_MODE_OUTPUT;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  
  config.pin_bit_mask = 1LL << (uint64_t) pin_last;
  gpio_config(&config);
  gpio_set_level(pin_last, 0);

  config.pin_bit_mask = 1LL << (uint64_t) pin_next;
  gpio_config(&config);
  gpio_set_level(pin_next, 0);
    
  config.pin_bit_mask = 1LL << (uint64_t) pin_led_gn;
  gpio_config(&config);
  gpio_set_level(pin_led_gn, 1);

  config.pin_bit_mask = 1LL << (uint64_t) pin_led_rd;
  gpio_config(&config);
  gpio_set_level(pin_led_rd, 1);

  // Init Timer
  const esp_timer_create_args_t timer_delay_args = {
    .callback = &delay_timer_callback,
    .name = "delay-timer"
  };
  esp_timer_create(&timer_delay_args, &delay_timer);

  const esp_timer_create_args_t timer_pulse_args = {
    .callback = &pulse_timer_callback,
    .name = "pulse-timer"
  };
  esp_timer_create(&timer_pulse_args, &pulse_timer);

  // Task
  control_evt_queue = xQueueCreate(10, sizeof(control_evt_t));
  xTaskCreate(&control_task, "control_task", 2048, NULL, 10, NULL);  
  xTaskCreate(&control_led_task, "control_led_task", 2048, NULL, 10, NULL);  
}
