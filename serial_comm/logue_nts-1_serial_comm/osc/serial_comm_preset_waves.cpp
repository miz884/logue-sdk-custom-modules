#include "serial_comm.h"

typedef struct State {
  const float * const * p;
  uint32_t max_w_index;
  uint32_t count;
  uint32_t w_index;
  uint32_t v_index;
} State;

static State state;

void init_message() {
  state.p = wavesA;
  state.max_w_index = 15;
  state.count = 0;
  state.w_index = 0;
  state.v_index = 0;
}

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len) {  
  (void) params;
}

Message get_next_message() {
  ++state.count;
  Message result;
  if (state.count < 10) {
    result.ui32 = 0L;
    return result;
  }
  if (state.w_index >= state.max_w_index) {
    result.ui32 = 0L;
    return result;
  }
  result.f = state.p[state.w_index][state.v_index++];
  if (state.v_index >= k_waves_size) {
    state.count =0;
    state.v_index = 0;
    state.w_index++;
  }
  return result;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  init_message();
}

