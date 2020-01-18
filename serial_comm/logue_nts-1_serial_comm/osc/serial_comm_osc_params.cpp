#include "serial_comm.h"

#define LENGTH (48000 / 64)

typedef struct State {
  uint32_t values[LENGTH];
  uint16_t w_index;
  uint16_t r_index;
  uint16_t count;
} State;

static State state;

void init_message() {
  state.w_index = 0;
  state.r_index = 0;
}

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len) {  
  if (state.w_index >= LENGTH) return;
  float v = q31_to_f32(params->shape_lfo);
  uint32_t ui = (uint32_t) ((v) * 10000.f);
  state.values[state.w_index++] = ui;
  // state.values[state.w_index++] = params->reserved0[2];
}

uint32_t get_next_message() {
  if (state.r_index == 0 && state.count++ < 10) return 0L;
  state.count = 0;
  uint32_t v = state.values[state.r_index++];
  state.r_index %= LENGTH;
  return v;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  init_message();
}
