#include "userrevfx.h"
#include "osc_api.h"

#define MAX_HZ (2000.f)

typedef struct State {
  float hz;
  float phase;
} State;

static State state;

void REVFX_INIT(uint32_t platform, uint32_t api) {
  state.phase = 0.f;
  state.hz = 440;
}

void REVFX_PROCESS(float *xn, uint32_t buf_len) {
  float * __restrict x = xn;
  const float * x_e = x + 2 * buf_len;

  const float w_delta = state.hz * k_samplerate_recipf;
  float phase = state.phase;

  while (x != x_e) {
    *(x++) = *(x++) = osc_softclipf(0.05f, osc_sinf(phase));
    phase += w_delta;
    phase -= (uint32_t) phase;
  }

  state.phase = phase;
}


void REVFX_PARAM(uint8_t index, int32_t value) {
  const float valf = q31_to_f32(value);
  switch (index) {
  case k_user_revfx_param_time:
    break;
  case k_user_revfx_param_depth:
    state.hz = MAX_HZ * valf;
    break;
  case k_user_revfx_param_shift_depth:
    break;
  default:
    break;
  }
}
