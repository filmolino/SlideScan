#ifndef __cable_release_codes_h_
#define __cable_release_codes_h_

#include "cable_release.h"

// Basic (Nikon getestet)
static const char *cable_release_codes_basic_name = "Basic";
static const uint8_t cable_release_codes_basic[] = {
  CABLE_RELEASE_FOCUS_ON,
  CABLE_RELEASE_DELAY,              /* Delay 500 ms */
  0xF4,
  0x01,
  CABLE_RELEASE_SHUTTER_ON,
  CABLE_RELEASE_DELAY,              /* Delay 200 ms */
  200,                            
  0,
  CABLE_RELEASE_FOCUS_OFF,
  CABLE_RELEASE_SHUTTER_OFF,
  CABLE_RELEASE_END
};

// Default
static const char *cable_release_codes_default_name = "Empty";
static const uint8_t cable_release_codes_default[] = {
  CABLE_RELEASE_END
};


#endif
