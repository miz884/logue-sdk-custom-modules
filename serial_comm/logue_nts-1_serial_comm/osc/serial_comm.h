#ifndef _SERIAL_COMM_H
#define _SERIAL_COMM_H

#include "osc_api.h"
#include "userosc.h"

typedef union message {
  float f;
  int32_t i32;
  uint32_t ui32;
} Message;

void init_message();

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len);

Message get_next_message();

#endif
