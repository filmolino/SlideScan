#ifndef __ir_release_codes_h_
#define __ir_release_codes_h_

#include "driver/rmt.h" 

// Nikon
static const char *ir_release_codes_nikon_name = "Nikon";
static const uint32_t ir_release_codes_nikon_freq = 38000;
static const uint8_t ir_release_codes_nikon_divider = 255;       // t = 1/(80 MHz / 255) = 0.0000031875
static const rmt_item32_t ir_release_codes_nikon[] = {
  {{{ 627, 1,  8730, 0 }}},   // Ton =   2ms, Toff 27.83ms  PERIOD1
  {{{ 125, 1,   496, 0 }}},   // Ton = 400us, Toff  1.58ms
  {{{ 125, 1,  1123, 0 }}},   // Ton = 400us, Toff  3.58ms
  {{{ 125, 1, 19827, 0 }}},   // Ton = 400us, Toff 63.20ms
  {{{ 627, 1,  8730, 0 }}},   // Ton =   2ms, Toff 27.83ms  PERIOD2
  {{{ 125, 1,   496, 0 }}},   // Ton = 400us, Toff  1.58ms
  {{{ 125, 1,  1123, 0 }}},   // Ton = 400us, Toff  3.58ms
  {{{ 125, 1, 19827, 0 }}},   // Ton = 400us, Toff 63.20ms
  {{{ 627, 1,  8730, 0 }}},   // Ton =   2ms, Toff 27.83ms  PERIOD3
  {{{ 125, 1,   496, 0 }}},   // Ton = 400us, Toff  1.58ms
  {{{ 125, 1,  1123, 0 }}},   // Ton = 400us, Toff  3.58ms
  {{{ 125, 1, 19827, 0 }}},   // Ton = 400us, Toff 63.20ms
  {{{ 0, 1, 0, 0 }}}          // RMT end marker
};

// Pentax 
static const char *ir_release_codes_pentax_name = "Pentax";
static const uint32_t ir_release_codes_pentax_freq = 38000;
static const uint8_t ir_release_codes_pentax_divider = 255;       // t = 1/(80 MHz / 255) = 0.0000031875
static const rmt_item32_t ir_release_codes_pentax[] = {
  {{{ 4078, 1, 941, 0 }}},    // Ton = 13ms, Toff 3ms
  {{{  314, 1, 314, 0 }}},    // Ton =  1ms, Toff 1ms
  {{{  314, 1, 314, 0 }}},    // Ton =  1ms, Toff 1ms
  {{{  314, 1, 314, 0 }}},    // Ton =  1ms, Toff 1ms
  {{{  314, 1, 314, 0 }}},    // Ton =  1ms, Toff 1ms
  {{{  314, 1, 314, 0 }}},    // Ton =  1ms, Toff 1ms
  {{{  314, 1, 314, 0 }}},    // Ton =  1ms, Toff 1ms
  {{{  314, 1, 314, 0 }}},    // Ton =  1ms, Toff 1ms
  {{{ 0, 1, 0, 0 }}}          // RMT end marker
};

// Olympus
static const char *ir_release_codes_olympus_name = "Olympus";
static const uint32_t ir_release_codes_olympus_freq = 40000;
static const uint8_t ir_release_codes_olympus_divider = 255;       // t = 1/(80 MHz / 255) = 0.0000031875
static const rmt_item32_t ir_release_codes_olympus[] = {
  {{{ 2815, 1, 1375, 0 }}},   // Ton = 8.972ms, Toff 4.384ms
  {{{  196, 1,    0, 0 }}},   // Ton = 0.624ms, Toff 0.000ms
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  153, 0,  188, 1 }}},   // Ton = 0.488ms, Toff 0.600ms (0)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{  502, 0,  188, 1 }}},   // Ton = 1.6ms,   Toff 0.600ms (1)
  {{{ 0, 1, 0, 0 }}}          // RMT end marker
};

// Minolta
static const char *ir_release_codes_minolta_name = "Minolta";
static const uint32_t ir_release_codes_minolta_freq = 38000;
static const uint8_t ir_release_codes_minolta_divider = 255;       // t = 1/(80 MHz / 255) = 0.0000031875
static const rmt_item32_t ir_release_codes_minolta[] = {
  {{{ 1176, 0,  593, 1 }}},   // Ton = 3.75ms,   Toff 1.89ms
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  153, 0 }}},   // Ton = 0.456ms,   Toff 0.487ms (0)
  {{{  143, 1,  449, 0 }}},   // Ton = 0.456ms,   Toff 1.430ms (1)
  {{{ 0, 1, 0, 0 }}}          // RMT end marker
};

// Sony
static const char *ir_release_codes_sony_name = "Sony";
static const uint32_t ir_release_codes_sony_freq = 40000;
static const uint8_t ir_release_codes_sony_divider = 255;       // t = 1/(80 MHz / 255) = 0.0000031875
static const rmt_item32_t ir_release_codes_sony[] = {
  {{{  728, 1,  204, 0 }}},   // Ton = 2.320ms, Toff 0.650ms
  {{{  728, 1,  204, 0 }}},   // Ton = 2.320ms, Toff 0.650ms
  {{{  728, 1,  204, 0 }}},   // Ton = 2.320ms, Toff 0.650ms
  {{{  728, 1,  204, 0 }}},   // Ton = 2.320ms, Toff 0.650ms
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  180, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (0)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{  367, 1,  204, 0 }}},   // Ton = 1.175ms, Toff 0.650ms (1)
  {{{ 0, 1, 0, 0 }}}          // RMT end marker
};

// Canon
static const char *ir_release_codes_canon_name = "Canon";
static const uint32_t ir_release_codes_canon_freq = 33000;
static const uint8_t ir_release_codes_canon_divider = 25;         // t = 1/(80 MHz / 25) = 0.0000003125
static const rmt_item32_t ir_release_codes_canon[] = {
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1, 23491, 0 }}},  // Ton = 0.011ms,   Toff 7.341ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{   35, 1,   35, 0 }}},   // Ton = 0.011ms,   Toff 0.011ms
  {{{ 0, 1, 0, 0 }}}          // RMT end marker
};

// Default
static const char *ir_release_codes_default_name = "Empty";
static const uint32_t ir_release_codes_default_freq = 38000;
static const uint8_t ir_release_codes_default_divider = 255;         // t = 1/(80 MHz / 25) = 0.0000003125
static const rmt_item32_t ir_release_codes_default[] = {
  {{{ 0, 1, 0, 0 }}}          // RMT end marker
};

#endif
