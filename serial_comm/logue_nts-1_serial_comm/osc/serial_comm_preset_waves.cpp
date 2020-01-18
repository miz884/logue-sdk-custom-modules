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

uint32_t get_next_message() {
  ++state.count;
  if (state.count < 10) return 0UL;
  // if (state.w_index >= (sizeof(state.p) / sizeof(state.p[0]))) return 0UL;
  if (state.w_index >= state.max_w_index) return 0UL;
  uint32_t result = (uint32_t) ((state.p[state.w_index][state.v_index++] + 1.f) * 10000.f);
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

