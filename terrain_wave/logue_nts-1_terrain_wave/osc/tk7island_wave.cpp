#include "userosc.h"
#include "../common/tk7island.h"
#include "../common/wavetables.h"

typedef struct State {
  float phase;
  uint8_t flags;
  uint8_t wavetable_index;
} State;

static State state;

enum {
  flags_clear = 0,
  flag_noteon = 1 << 0,
  flag_noteoff = 1 << 1,
};

void OSC_INIT(uint32_t platform, uint32_t api) {
  state.phase = 0.f;
  state.flags = flags_clear;
  state.wavetable_index = 0;

  init_wavetables();
}

void OSC_CYCLE(const user_osc_param_t* params,
               int32_t* yn,
               const uint32_t frames) {  
  // Handle the reset flag on NOTEON.
  if (state.flags & flag_noteon) {
    state.flags &= ~(flag_noteon);
    state.phase = 0.f;
  }

  // Restore the last state.
  float phase = state.phase;

  // Calculate the current phase.
  const float w_delta = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);

  // Prepare the result buffer.
  q31_t* __restrict y = (q31_t*) yn;
  const q31_t* y_e = y + frames;

  for (; y != y_e; ) {

    // Main signal
    const float p = ((phase < .5f) ? phase : 1.f - phase) * 2.f;
    float sig = get_wave_value(state.wavetable_index, p);
    sig = (sig + 1.f) / 2.f; // 0.0 <= sig <= 1.0
    sig *= (phase < .5f) ? 1.f : -1.f; // -1.0 <= sig <= 1.0
    *(y++) = f32_to_q31(osc_softclipf(0.05f, sig));

    // Next step.
    phase += w_delta;
    phase -= (uint32_t) phase; // to keep phase within 0.0-1.0.
  }

  // Store the state.
  state.phase = phase;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  state.flags |= flag_noteon;
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
  state.flags |= flag_noteoff;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
  case k_user_osc_param_id1:
    state.wavetable_index = value;
    break;
  case k_user_osc_param_id2:
  case k_user_osc_param_id3:
  case k_user_osc_param_id4:
  case k_user_osc_param_id5:
  case k_user_osc_param_id6:
  case k_user_osc_param_shape:
  case k_user_osc_param_shiftshape:
  default:
    break;
  }
}

