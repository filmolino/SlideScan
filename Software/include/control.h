#ifndef _CONTROL_H__
#define _CONTROL_H__

#include "driver/gpio.h"

// STATES
typedef enum {
  CONTROL_STATE_IDLE = 0,
  CONTROL_STATE_ERROR = 1,
  CONTROL_STATE_NEXT = 2,
  CONTROL_STATE_LAST = 3,
  CONTROL_STATE_SCAN = 4,
} control_states_t;

// EVENTS
typedef enum {
  CONTROL_EVT_NONE = 0,
  CONTROL_EVT_NEXT = 1,
  CONTROL_EVT_LAST = 2,
  CONTROL_EVT_START = 3,
  CONTROL_EVT_STOP = 4,
  CONTROL_EVT_SCAN = 5,
  CONTROL_EVT_BTN_INTERNAL_SHORT = 6,
  CONTROL_EVT_BTN_INTERNAL_LONG = 7,
  CONTROL_EVT_TIMER = 8,
  CONTROL_EVT_RESET_CURRENT_SLIDES = 9,
  CONTROL_EVT_ERROR = 10,
} control_evt_t;

QueueHandle_t control_get_queue();
void control_set_cable_release_queue();
bool control_is_scanning();
void control_set_current_slide(char *payload);
void control_reset_current_slide();
void control_get_current_slide(char *payload);
void control_init(gpio_num_t last, gpio_num_t next, gpio_num_t led_gn, gpio_num_t led_rd);

#endif
