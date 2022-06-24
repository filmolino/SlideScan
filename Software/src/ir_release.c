#include "ir_release.h"

static rmt_channel_t ir_channel;

void ir_release_init(gpio_num_t pin, rmt_channel_t channel, uint8_t divider, uint32_t freq) {
  ir_channel = channel;
  rmt_config_t rmt = RMT_DEFAULT_CONFIG_TX(pin, ir_channel);
  rmt.tx_config.carrier_en = true;
  rmt.tx_config.carrier_duty_percent = 50;
  rmt.tx_config.carrier_freq_hz = freq;
  rmt.clk_div = divider;     
  ESP_ERROR_CHECK(rmt_config(&rmt));
  ESP_ERROR_CHECK(rmt_driver_install(rmt.channel, 0, 0));
}

void ir_release_send(rmt_item32_t *item) {
  rmt_write_items(ir_channel, item, IR_RELEASE_SEQ_SIZE, false);
}