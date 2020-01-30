#ifndef _SERIAL_COMM_MODFX_H
#define _SERIAL_COMM_MODFX_H

#include "usermodfx.h"

#ifndef k_samplerate
#define k_samplerate        (48000)
#endif

#ifndef k_samplerate_recipf
#define k_samplerate_recipf (2.08333333333333e-005f)
#endif

typedef union message {
  float f;
  int32_t i32;
  uint32_t ui32;
} Message;

void init_message();

void update_message(const float *main_xn, float *main_yn,
                    const float *sub_xn,  float *sub_yn,
                    uint32_t frames);

Message get_next_message();

#endif
