#ifndef __ir_release_h_
#define __ir_release_h_

#include "driver/rmt.h"
#include "driver/gpio.h"

#define IR_RELEASE_SEQ_SIZE       40
#define IR_RELEASE_CODES_SIZE     32

typedef struct {
  char *name;
  rmt_item32_t ir_seq[IR_RELEASE_SEQ_SIZE];
  uint32_t freq;
  uint8_t divider;
} ir_release_seq_t;

void ir_release_send(rmt_item32_t *item);
void ir_release_init(gpio_num_t pin, rmt_channel_t channel, uint8_t divider, uint32_t freq);

#endif
