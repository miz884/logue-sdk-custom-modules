#include "serial_comm_revfx.h"

typedef struct State {
  Message value;
} State;

static State state;

void init_message() {
  state.value.ui32 = 0L;
}

void update_message(const float *xn, uint32_t buf_len) {
  (void) buf_len;
}

Message get_next_message() {
  return state.value;
}

void REVFX_PARAM(uint8_t index, int32_t value) {
  const float f = q31_to_f32(value);
  switch (index) {
  case k_user_revfx_param_time:
  case k_user_revfx_param_depth:
  case k_user_revfx_param_shift_depth:
    state.value.f = f;
    break;
  default:
    break;
  }
}
