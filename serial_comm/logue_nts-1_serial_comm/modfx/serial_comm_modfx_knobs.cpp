#include "serial_comm_modfx.h"

typedef struct State {
    uint32_t value;
} State;

static State state;

void init_message() {
  state.value = 0;
}

void update_message(const float *main_xn, float *main_yn,
                    const float *sub_xn,  float *sub_yn,
                    uint32_t buf_len) {
  (void) buf_len;
}

uint32_t get_next_message() {
  return state.value;
}

void MODFX_PARAM(uint8_t index, int32_t value) {
  float f = q31_to_f32(value);
  switch (index) {
  case k_user_modfx_param_time:
  case k_user_modfx_param_depth:
    break;
  case k_num_user_modfx_param_id:
    state.value = (uint32_t) (f * 10000.f);;
    break;
  default:
    break;
  }
}
