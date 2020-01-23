#ifndef _MT_H
#define _MT_H

#include <stdint.h>

#define NUM_WAVETABLES 2

#ifdef SIMPLE_WAVETABLES
extern uint16_t wavetable0[];
extern uint16_t wavetable1[];
#else
extern float wavetable0[];
extern float wavetable1[];
#endif
extern uint16_t wavetable0_len;
extern uint16_t wavetable1_len;

#endif
