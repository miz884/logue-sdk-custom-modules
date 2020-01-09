#include "userosc.h"

extern float wavetable0[];
extern float wavetable1[];
extern uint16_t wavetable0_len;
extern uint16_t wavetable1_len;

extern uint16_t wavetable_len[2];
extern float* wavetables[2];

extern void init_wavetables();

extern float get_wave_value(const uint16_t w_index, const float phase);

