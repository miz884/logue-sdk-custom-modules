#include "userosc.h"

typedef struct State {
  float phase;
  uint16_t flags;
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

  // Calculate the phase delta.
  const float w_delta = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);

  // Prepare the result buffer.
  q31_t* __restrict y = (q31_t*) yn;
  const q31_t* y_e = y + frames;

  while (y != y_e) {
    // LFO
    const float lfo = 1.f - q31_to_f32(params->shape_lfo);

    // Main signal
    float sig  = osc_sawf(phase);
    sig  = osc_softclipf(0.05f, lfo * sig); 
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

