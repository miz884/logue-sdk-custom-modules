#include "userosc.h"
#include "wavetables.h"

typedef struct State {
  float phase;
  uint8_t flags;
  uint8_t wavetable_index;
  uint32_t step_count;
  uint8_t eg_enabled;
  float eg_scale;
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
  state.step_count = 0;
  state.eg_enabled = 0;
  state.eg_scale = 1.0;

  init_wavetables();
}

float get_signal_from_wavetable(const float phase, const float w_delta) {
  const uint16_t index = (uint16_t) ((float) wavetable_len[state.wavetable_index] * (phase - (uint16_t) phase));
  return wavetables[state.wavetable_index][index];
}

void OSC_CYCLE(const user_osc_param_t* params,
               int32_t* yn,
               const uint32_t frames) {  
  // Handle the reset flag on NOTEON.
  if (state.flags & flag_noteon) {
    state.flags = flags_clear;
    state.phase = 0.f;
    state.step_count = 0;
  }

  // Restore the last state.
  float phase = state.phase;
  uint32_t steps = state.step_count;
  uint8_t eg_enabled = state.eg_enabled;
  float eg_scale = state.eg_scale;

  // Calculate the current phase.
  const float w_delta = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);

  // Prepare the result buffer.
  q31_t* __restrict y = (q31_t*) yn;
  const q31_t* y_e = y + frames;

  for (; y != y_e; ) {

    // Main signal
    const float drive = (get_signal_from_wavetable(((float) steps / k_samplerate * eg_scale), w_delta) + 1.0) / 2.0;
    const float sig  = osc_softclipf(0.05f, (eg_enabled == 1 ? drive : 1.0) * get_signal_from_wavetable(phase, w_delta));
    *(y++) = f32_to_q31(sig);

    // Next step.
    phase += w_delta;
    phase -= (uint32_t) phase; // to keep phase within 0.0-1.0.
    if (steps < k_samplerate / eg_scale) ++steps;
  }

  // Store the state.
  state.phase = phase;
  state.step_count = steps;
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
    state.eg_enabled = value;
    break;
  case k_user_osc_param_id3:
    state.eg_scale = (float) value;
    break;
  case k_user_osc_param_id4:
  case k_user_osc_param_id5:
  case k_user_osc_param_id6:
  case k_user_osc_param_shape:
  case k_user_osc_param_shiftshape:
  default:
    break;
  }
}

