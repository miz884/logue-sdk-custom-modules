#ifndef _SERIAL_COMM_DELFX_H
#define _SERIAL_COMM_DELFX_H

#include "userdelfx.h"

typedef union message {
  float f;
  int32_t i32;
  uint32_t ui32;
} Message;

void init_message();

void update_message(const float *xn, uint32_t frames);

Message get_next_message();

#endif
