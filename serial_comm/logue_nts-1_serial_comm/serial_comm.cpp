#include "userosc.h"

typedef struct State {
  uint16_t phase;
  uint8_t clock;
  uint8_t message_position;
  int val;
  uint32_t msg;
} State;

static State s_state;

#define HIGH 0.99f
#define ZERO 0.f
#define LOW -0.99f

#define CLOCK_PER_SEC (80)
#define SIGNAL_PER_SEC (40)
static int clock_len = (int) (48000 / CLOCK_PER_SEC);
static int sig_len = (int) (48000 / SIGNAL_PER_SEC);
static int message_len = 32;

void OSC_INIT(uint32_t platform, uint32_t api) {
  s_state.phase = 0;
  s_state.clock = 0;
  s_state.message_position = 0;
  s_state.msg = 0;
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames) {  
  // Prepare the result buffer.
  q31_t * __restrict y = (q31_t *)yn;
  const q31_t * y_e = y + frames;

  // Restore the last state.
  uint16_t phase = s_state.phase;
  uint8_t clock = s_state.clock;
  uint8_t m_pos = s_state.message_position;
  uint32_t msg = s_state.msg;
 
  for (; y != y_e; ) {
    // Send the stop frames in last 16 clock frames.
    if (clock >= 64) {
      switch(clock) {
        case 64:
          *(y++) = f32_to_q31(HIGH);
          break;
        case 65:
          *(y++) = f32_to_q31(LOW);
          break;
        case 66:
        case 67:
        case 68:
        case 69:
          *(y++) = f32_to_q31(HIGH);
          break;
        case 70:
        case 71:
        case 72:
        case 73:
        case 74:
        case 75:
        case 76:
        case 77:
        case 78:
          *(y++) = f32_to_q31(LOW);
          break;
        case 79:
          *(y++) = f32_to_q31(HIGH);
          break;
      }
    } else {
      // Otherwise, send the signal.
      int bit = ((msg & (1 << m_pos)) > 0) ? 1 : 0;
      *(y++) = f32_to_q31(((clock % 2) == bit) ? HIGH : LOW);
    }

    // Next step.
    ++phase;
    phase %= 48000;
    if (phase % clock_len == 0) ++clock;
    if (phase % sig_len == 0) --m_pos;
    // Reset counters every seconds.
    if (phase == 0) {
      m_pos = message_len - 1;
      clock = 0;
      msg = s_state.val;
    }
  }

  // Store the state.
  s_state.phase = phase;
  s_state.clock = clock;
  s_state.message_position = m_pos;
  s_state.msg = msg;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  (void)params;
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  const float valf = param_val_to_f32(value);
  const int val = value;
  
  switch (index) {
  case k_user_osc_param_id1:
    s_state.val = val;
    break;
  case k_user_osc_param_id2:
    s_state.val = val;
    break;
  case k_user_osc_param_id3:
    s_state.val = val;
    break;
  case k_user_osc_param_id4:
    s_state.val = val;
    break;
  case k_user_osc_param_id5:
    s_state.val = val;
    break;
  case k_user_osc_param_id6:
    s_state.val = val;
    break;
  case k_user_osc_param_shape:
    s_state.val = val;
    break;
  case k_user_osc_param_shiftshape:
    s_state.val = val;
    break;
  default:
    break;
  }
}

