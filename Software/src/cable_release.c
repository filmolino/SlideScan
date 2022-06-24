#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "cable_release.h"

static QueueHandle_t cable_release_evt_queue = NULL;

// LOCALS
static const char *TAG = "cable_release";
static gpio_num_t pin_shutter = 0;
static gpio_num_t pin_focus = 0;
static uint8_t current_cmd = 0;
static uint8_t *current_seq = NULL;
static bool seq_active = false;

// FUNCTIONS

void cable_release_task(void *param) {
  cable_release_evt_t evt;
  uint32_t delay;

  ESP_LOGI(TAG, "cable_release_task created");

  while (1) {
    if (xQueueReceive(cable_release_evt_queue, (void*) &evt, 10)) {      
      if (evt == CABLE_RELEASE_EVT_START) {
        current_cmd = 0;
        seq_active = true;

        ESP_LOGI(TAG, "starting sequence ptr=%u  |  val=%u", current_seq, *current_seq);

        if (!current_seq) {
          ESP_LOGE(TAG, "ERROR no valid buffer");            
          seq_active = false;       /* no valid buffer */
        }

        while ((current_cmd < CABLE_RELEASE_SEQ_SIZE) && (seq_active))
        {
          switch(*(current_seq + current_cmd++) ) {
            case CABLE_RELEASE_FOCUS_ON:
              ESP_LOGI(TAG, "CMD Focus ON");
              gpio_set_level(pin_focus, 1);
              break;

            case CABLE_RELEASE_FOCUS_OFF:
              ESP_LOGI(TAG, "CMD Focus OFF");            
              gpio_set_level(pin_focus, 0);
              break;

            case CABLE_RELEASE_SHUTTER_ON:
              ESP_LOGI(TAG, "CMD Shutter ON");            
              gpio_set_level(pin_shutter, 1);
              break;

            case CABLE_RELEASE_SHUTTER_OFF:
              ESP_LOGI(TAG, "CMD Shutter OFF");            
              gpio_set_level(pin_shutter, 0);
              break;

            case CABLE_RELEASE_DELAY:
              delay = *(current_seq + current_cmd++);
              delay |= *(current_seq + current_cmd++) << 8;
              vTaskDelay(delay / portTICK_PERIOD_MS);
              break;

            case CABLE_RELEASE_END:
              seq_active = false;
              break;
            
            default:
              seq_active = false;
              break;
          }
        }
      }
    }
  }
}


// Sets pointer to current sequence
void cable_release_set_seq(uint8_t *seq) {
  current_seq = seq;
  ESP_LOGI(TAG, "current_seq = %d", *current_seq);
}


// Get ptr to event queue
QueueHandle_t cable_release_get_queue_ptr() {
  return cable_release_evt_queue;
}


void cable_release_init(gpio_num_t shutter, gpio_num_t focus) {
  // Variables
  current_cmd = 0;
  seq_active = false;

  // GPIO
  pin_shutter = shutter;
  pin_focus = focus;
  gpio_config_t config = {};
  config.mode = GPIO_MODE_OUTPUT;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  config.pin_bit_mask = 1LL << (uint64_t) pin_shutter;
  gpio_config(&config);
  config.pin_bit_mask = 1LL << (uint64_t) pin_focus;
  gpio_config(&config);
  gpio_set_level(pin_shutter, 0);
  gpio_set_level(pin_focus, 0);

  // Task
  cable_release_evt_queue = xQueueCreate(10, sizeof(cable_release_evt_t));

  ESP_LOGI(TAG, "cable_release_evt_queue %u", cable_release_evt_queue);


  xTaskCreate(&cable_release_task, "cable_release_task", 2048, NULL, 10, NULL);
}
