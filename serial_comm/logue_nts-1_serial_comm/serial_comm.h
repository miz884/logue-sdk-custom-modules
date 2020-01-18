#ifndef _SERIAL_COMM_H
#define _SERIAL_COMM_H

#include "osc_api.h"
#include "userosc.h"

void init_message();

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len);

uint32_t get_next_message();

#endif
