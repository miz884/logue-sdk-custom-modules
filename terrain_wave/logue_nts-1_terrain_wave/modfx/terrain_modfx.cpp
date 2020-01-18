#include "usermodfx.h"
#include "../common/wavetables.h"

#ifndef k_samplerate
#define k_samplerate        (48000)
#endif

#ifndef k_samplerate_recipf
#define k_samplerate_recipf (2.08333333333333e-005f)
#endif

typedef struct State {
  float phase;

  float param_time;
  float param_depth;
  uint16_t wavetable_index;
} State;

static State state;

void MODFX_INIT(uint32_t platform, uint32_t api) {
  state.phase = 0.f;
  state.param_time = 0.f;
  state.param_depth = 0.f;
  state.wavetable_index = 0;

  init_wavetables();
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn,  float *sub_yn,
                   uint32_t frames) {
  // Prepare the result buffer.
  const float *mx = main_xn;
  float * __restrict my = main_yn;
  const float * my_e = my + 2 * frames;

  const float *sx = sub_xn;
  float * __restrict sy = sub_yn;

  // Restore the last state.
  float phase = state.phase;
  const float p_time = state.param_time;
  const float p_depth = state.param_depth;

  while (my != my_e) {
    float wave = get_wave_value(state.wavetable_index, phase);
    wave = (wave + 1.f) / 2.f * p_depth + (1.f - p_depth / 2.f);;
    
    *(my++) = wave * *(mx++);
    *(my++) = wave * *(mx++);
    *(sy++) = wave * *(sx++);
    *(sy++) = wave * *(sx++);

    // Next step.
    phase += k_samplerate_recipf * (p_time * 9.f + 1.f);
    phase -= (uint32_t) phase;
  }

  // Store the state.
  state.phase = phase;
}


void MODFX_PARAM(uint8_t index, int32_t value) {
  const float valf = q31_to_f32(value);
  switch (index) {
  case k_user_modfx_param_time:
    state.param_time = valf;
    break;
  case k_user_modfx_param_depth:
    state.param_depth = valf;
    break;
  default:
    break;
  }
}

