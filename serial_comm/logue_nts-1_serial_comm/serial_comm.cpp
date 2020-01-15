#include "userosc.h"

typedef struct State {
  uint16_t frames;
  uint16_t clock;
  uint16_t msg_bit_pos;
  uint32_t msg;

  uint16_t count;
  uint16_t min;
  uint16_t max;
} State;

static State s_state;

#define HIGH 0.99f
#define ZERO 0.f
#define LOW -0.99f

// 48000 (frames per second) / 30 (frames per clock) = 1,600 clocks per second.
#define FRAMES_PER_CLOCK (40)

#define MSG_LEN (32)

const float sync_signals[] = {
  HIGH, HIGH, HIGH, LOW, HIGH
};

const uint16_t sync_signals_len = sizeof(sync_signals) / sizeof(sync_signals[0]);

uint32_t get_next_message() {
  ++s_state.count;
  if (s_state.count % 2 == 0) {
    return s_state.max;
  } else {
    return s_state.min;
  }
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  s_state.frames = 0;
  s_state.clock = 0;
  s_state.msg_bit_pos = 0;
  s_state.msg = 0;

  s_state.count = 0;
  s_state.min = 0xFFFF;
  s_state.max = 0;
}

void OSC_CYCLE(const user_osc_param_t *params,
               int32_t *yn,
               const uint32_t buf_len) {  
  // Prepare the result buffer.
  q31_t * __restrict y = (q31_t *) yn;
  const q31_t * y_e = y + buf_len;

  // Restore the last state.
  uint16_t frames = s_state.frames;
  uint16_t clock = s_state.clock;
  uint16_t msg_bit_pos = s_state.msg_bit_pos;
  uint32_t msg = s_state.msg;

  s_state.max = (buf_len > s_state.max) ? buf_len : s_state.max;
  s_state.min = (buf_len < s_state.min) ? buf_len : s_state.min;
 
  for (; y != y_e; ) {
    if (clock < MSG_LEN * 2) {
      // Send the msg.
      int bit = ((msg & (1 << msg_bit_pos)) > 0) ? 1 : 0;
      *(y++) = f32_to_q31(((clock % 2) == bit) ? HIGH : LOW);
    } else {
      // Send the sync signal in last 16 clocks.
      *(y++) = f32_to_q31(sync_signals[clock - MSG_LEN * 2]);
    }

    // Next frame.
    ++frames;
    if (frames % FRAMES_PER_CLOCK == 0) {
      // Next clock.
      frames = 0;
      ++clock;
      if (clock % 2 == 0 && msg_bit_pos > 0) {
        --msg_bit_pos;
      }
      if (clock % (MSG_LEN * 2 + sync_signals_len) == 0) {
        // Next message.
        clock = 0;
        msg_bit_pos = MSG_LEN - 1;
        msg = get_next_message();
      }
    }
  }

  // Store the state.
  s_state.frames = frames;
  s_state.clock = clock;
  s_state.msg_bit_pos = msg_bit_pos;
  s_state.msg = msg;
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  (void) params;
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
  (void) params;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  
  switch (index) {
  case k_user_osc_param_id1:
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

