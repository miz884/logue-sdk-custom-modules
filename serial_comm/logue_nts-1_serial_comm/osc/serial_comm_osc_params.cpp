#include "serial_comm.h"

#define LENGTH (48000 / 64)

typedef struct State {
  Message values[LENGTH];
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
  state.values[state.w_index++].f = q31_to_f32(params->shape_lfo);
}

Message get_next_message() {
  Message v;
  if (state.r_index == 0 && state.count++ < 10) {
    v.ui32 = 0L;
    return v;
  }
  state.count = 0;
  v = state.values[state.r_index++];
  state.r_index %= LENGTH;
  return v;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  init_message();
}
