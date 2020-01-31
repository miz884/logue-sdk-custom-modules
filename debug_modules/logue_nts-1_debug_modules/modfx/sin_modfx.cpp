#include "usermodfx.h"
#include "osc_api.h"

#define MAX_HZ (2000.f)

typedef struct State {
  float hz;
  float phase;
} State;

static State state;

void MODFX_INIT(uint32_t platform, uint32_t api) {
  state.phase = 0.f;
  state.hz = 440;
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn,  float *sub_yn,
                   uint32_t buf_len) {
  const float *mx = main_xn;
  float * __restrict my = main_yn;
  const float * my_e = my + 2 * buf_len;

  const float *sx = sub_xn;
  float * __restrict sy = sub_yn;

  const float w_delta = state.hz * k_samplerate_recipf;
  float phase = state.phase;

  while (my != my_e) {
    *(my++) = *(my++) = *(sy++) = *(sy++) = osc_softclipf(0.05f, osc_sinf(phase));
    phase += w_delta;
    phase -= (uint32_t) phase;
  }

  state.phase = phase;
}

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float valf = q31_to_f32(value);
  switch (index) {
  case k_user_modfx_param_time:
    break;
  case k_user_modfx_param_depth:
    state.hz = MAX_HZ * valf;
    break;
  default:
    break;
  }
}
