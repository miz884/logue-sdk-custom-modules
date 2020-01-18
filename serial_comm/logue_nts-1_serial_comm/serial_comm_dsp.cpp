#include "serial_comm.h"
#include "simplelfo.hpp"

#define LENGTH 128

typedef struct State {
  uint32_t values[LENGTH];
  uint16_t index;
} State;

static State state;
static dsp::SimpleLFO lfo;

void init_message() {
  state.index = 0;
  lfo.reset();
  lfo.setF0(48000.f / (float) LENGTH, 1.f / 48000.f);
  q31_t phi0 = 0x80000000;
  q31_t w0 = f32_to_q31(2.f * 48000.f / (float) LENGTH *  1.f / 48000.f);
  for (int i = 0; i < LENGTH; ++i) {
    const float phif = q31_to_f32(phi0);
    // const float v = phif;
    // const float v = si_fabsf(phif);
    // const float v = (si_fabsf(phif) - 1.f);
    // const float v = phif * (si_fabsf(phif) - 1.f);
    const float v = lfo.sine_bi();
    state.values[i] = (uint32_t) ((v + 1.f) * 10000.f);
    phi0 += w0;
    lfo.cycle();
  }
}

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len) {  
  (void) params;
}

uint32_t get_next_message() {
  uint32_t v = state.values[state.index++];
  state.index %= LENGTH;
  return v;
}
