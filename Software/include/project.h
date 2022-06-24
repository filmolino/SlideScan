#ifndef __project_h_
#define __project_h_

// GPIO
#define GPIO_BTN1                   GPIO_NUM_8        // Short: NEXT Slide, Long: LAST Slide
#define GPIO_BTN2                   GPIO_NUM_9        // Short: SCAN Slide (STOP AUTO SCAN), Long: START AUTO SCAN
#define GPIO_INTERNAL_LED           GPIO_NUM_2
#define GPIO_INTERNAL_BUTTON        GPIO_NUM_3 
#define GPIO_RELAY_LAST             GPIO_NUM_7
#define GPIO_RELAY_NEXT             GPIO_NUM_1
#define GPIO_CABLE_RELEASE_FOCUS    GPIO_NUM_6
#define GPIO_CABLE_RELEASE_SHUTTER  GPIO_NUM_4
#define GPIO_IR_RELEASE             GPIO_NUM_0
#define GPIO_LED_GREEN              GPIO_NUM_5       
#define GPIO_LED_RED                GPIO_NUM_10

// BUTTONS
#define BTN_POLL_PERIOD             25                        // 25 ms
#define BTN_SHORT_PRESS_TIME_MAX    (500 / BTN_POLL_PERIOD)   // 500 ms

// SPIFFS
#define SPIFFS_PATH                 "/spiffs"

// WiFi
#define WIFI_CHANNEL                1
#define WIFI_STA_MAX_CONN           10

// LED
#define LED_BLINK_PERIOD            (500 / portTICK_PERIOD_MS)  // 500ms
#define LED_BLINK_SLOW_FACTOR       6                           // Slow blink = 6 * 500 ms = 3 s

#endif
