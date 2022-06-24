#ifndef __cable_release_h_
#define __cable_release_h_

#include "driver/gpio.h"

#define CABLE_RELEASE_SEQ_SIZE      32
#define CABLE_RELEASE_CODES_SIZE    32

// Sequence Commands
#define CABLE_RELEASE_END           0
#define CABLE_RELEASE_FOCUS_ON      1
#define CABLE_RELEASE_FOCUS_OFF     2
#define CABLE_RELEASE_SHUTTER_ON    3
#define CABLE_RELEASE_SHUTTER_OFF   4
#define CABLE_RELEASE_DELAY         5


typedef struct {
  char *name;
  uint8_t cable_seq[CABLE_RELEASE_SEQ_SIZE];
} cable_release_seq_t;


// EVENTS
typedef enum {
  CABLE_RELEASE_EVT_NONE = 0,
  CABLE_RELEASE_EVT_START = 1,
} cable_release_evt_t;

// EXPORTED FUNCTIONS
// Sets pointer to current sequence
void cable_release_set_seq(uint8_t *seq);
QueueHandle_t cable_release_get_queue_ptr();
void cable_release_init(gpio_num_t pinShutter, gpio_num_t pinFocus);

#endif
