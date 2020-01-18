#include "userosc.h"

typedef struct State {
  float phase;
  uint16_t flags;

  const float * const * wavetable;
  uint16_t wavetable_index;
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

  state.wavetable = wavesA;
  state.wavetable_index = 0;
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
  const float * const * wavetable = state.wavetable;
  const uint16_t wavetable_index = state.wavetable_index;

  // Calculate the phase delta.
  const float w_delta = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);

  // Prepare the result buffer.
  q31_t* __restrict y = (q31_t*) yn;
  const q31_t* y_e = y + frames;

  while (y != y_e) {
    // Main signal
    float sig  = osc_wave_scanf(wavetable[wavetable_index], phase);
    sig  = osc_softclipf(0.05f, sig);
    *(y++) = f32_to_q31(sig);

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
    state.wavetable = wavesA;
    state.wavetable_index = value;
    break;
  case k_user_osc_param_id2:
    state.wavetable = wavesB;
    state.wavetable_index = value;
    break;
  case k_user_osc_param_id3:
    state.wavetable = wavesC;
    state.wavetable_index = value;
    break;
  case k_user_osc_param_id4:
    state.wavetable = wavesD;
    state.wavetable_index = value;
    break;
  case k_user_osc_param_id5:
    state.wavetable = wavesE;
    state.wavetable_index = value;
    break;
  case k_user_osc_param_id6:
    state.wavetable = wavesF;
    state.wavetable_index = value;
    break;
  case k_user_osc_param_shape:
  case k_user_osc_param_shiftshape:
  default:
    break;
  }
}

