#include "serial_comm.h"

typedef struct SerialCommState {
  uint16_t frames;
  uint16_t clock;
  uint16_t msg_bit_pos;
  Message msg;
} SerialCommState;

static SerialCommState sc_state;

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

void OSC_INIT(uint32_t platform, uint32_t api) {
  sc_state.frames = 0;
  sc_state.clock = 0;
  sc_state.msg_bit_pos = 0;
  sc_state.msg.ui32 = 0;

  init_message();
}

void OSC_CYCLE(const user_osc_param_t *params,
               int32_t *yn,
               const uint32_t buf_len) {  
  update_message(params, yn, buf_len);

  // Prepare the result buffer.
  q31_t * __restrict y = (q31_t *) yn;
  const q31_t * y_e = y + buf_len;

  // Restore the last state.
  uint16_t frames = sc_state.frames;
  uint16_t clock = sc_state.clock;
  uint16_t msg_bit_pos = sc_state.msg_bit_pos;
  Message msg = sc_state.msg;

  while (y != y_e) {
    if (clock < MSG_LEN * 2) {
      // Send the msg.
      int bit = ((msg.ui32 & (1 << msg_bit_pos)) > 0) ? 1 : 0;
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
  sc_state.frames = frames;
  sc_state.clock = clock;
  sc_state.msg_bit_pos = msg_bit_pos;
  sc_state.msg = msg;
}

