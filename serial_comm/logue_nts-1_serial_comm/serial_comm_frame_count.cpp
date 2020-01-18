#include "serial_comm.h"

typedef struct State {
  uint32_t count;
  uint16_t min;
  uint16_t max;
} State;

static State state;

void init_message() {
  state.count = 0;
  state.min = 0xFFFF;
  state.max = 0;
}

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len) {  
  state.min = (buf_len < state.min) ? buf_len : state.min;
  state.max = (buf_len > state.max) ? buf_len : state.max;
}

uint32_t get_next_message() {
  ++state.count;
  state.count %= 2;
  if (state.count == 0) {
    return state.min;
  } else {
    return state.max;
  }
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  init_message();
}

