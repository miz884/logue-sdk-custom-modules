#include "serial_comm_modfx.h"

typedef struct State {
  Message value;
} State;

static State state;

void init_message() {
  state.value.ui32 = 0L;
}

void update_message(const float *main_xn, float *main_yn,
                    const float *sub_xn,  float *sub_yn,
                    uint32_t buf_len) {
  (void) buf_len;
}

Message get_next_message() {
  return state.value;
}

void MODFX_PARAM(uint8_t index, int32_t value) {
  float f = q31_to_f32(value);
  switch (index) {
  case k_user_modfx_param_time:
  case k_user_modfx_param_depth:
    state.value.f = f;
    break;
  case k_num_user_modfx_param_id:
    break;
  default:
    break;
  }
}
