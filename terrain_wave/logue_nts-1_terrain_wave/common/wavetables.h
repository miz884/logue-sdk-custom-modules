#include <stdint.h>

#ifndef _WAVETABLES_H
#define _WAVETABLES_H

#ifdef SIMPLE_WAVETABLES
extern uint16_t* wavetables[NUM_WAVETABLES];
#else
extern float* wavetables[NUM_WAVETABLES];
#endif
extern uint16_t wavetable_len[NUM_WAVETABLES];

extern void init_wavetables();

extern float get_wave_value(const uint16_t w_index, const float phase);

#endif
