#define param_val_to_f32(val) ((uint16_t) val * 9.77517106549365e-004f)  

#define SIGNAL_LEVEL_THRESHOLD (50)

#define FRAMES_PER_CLOCK (40.f)

#define MICROS_PER_FRAME (1.f / 48000.f * 1000.f * 1000.f)
#define MICROS_PER_CLOCK (MICROS_PER_FRAME * FRAMES_PER_CLOCK)
#define SYNC_SIGNAL_CLOCKS (3)
#define CLOCKS_TO_SKIP (2)

#define ACCEPTABLE_ERROR (0.90f)
#define SINGLE_CLOCKS_MICROS (MICROS_PER_CLOCK * 1.f * ACCEPTABLE_ERROR)
#define DOUBLE_CLOCKS_MICROS (MICROS_PER_CLOCK * 2.f * ACCEPTABLE_ERROR)
#define SYNC_SIGNAL_MICROS (MICROS_PER_CLOCK * (float) SYNC_SIGNAL_CLOCKS * ACCEPTABLE_ERROR)

typedef union Message {
  float f;
  int32_t i32;
  uint32_t ui32;
  int16_t i16;
} Message;

typedef struct State {
  uint16_t prev_sig = 0;
  unsigned long prev_clock_micros = 0;
  unsigned long skip_until = 0;

  uint32_t result[3] = {0, 0, 0};
  uint16_t result_current_pos = 0;
} State;
static State state;

void result_reset() {
  state.result[0] = 0;
  state.result[1] = 0;
  state.result[2] = 0;
  state.result_current_pos = 0;
}

uint16_t result_get(const uint16_t pos) {
  const uint16_t p0 = (uint16_t) (pos / 32);
  const uint16_t p1 = pos % 32;
  return ((state.result[p0] & (1UL << p1)) != 0) ? 1 : 0;
}

void result_push(const uint16_t val) {
  ++(state.result_current_pos);
  if (val > 0) {
    const uint16_t p0 = (uint16_t) (state.result_current_pos / 32);
    const uint16_t p1 = state.result_current_pos % 32;
    state.result[p0] |= (1UL << p1);
  }
}

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
  
  state.prev_sig = 0;
  state.prev_clock_micros = 0;
  state.skip_until = 0;
  result_reset();
}

/*
 * Decode a Manchester code.
 */
Message decode(const int offset) {
  Message output;
  output.ui32 = 0L;
  for (int i = offset; i < state.result_current_pos; i += 2) {
    const uint16_t b0 = result_get(i);
    const uint16_t b1 = result_get(i + 1);
    // Same signal can't appear in 2 frames in a row.
    if (b0 == b1) {
      if (offset == 0) {
        // Try to shift a frame.
        return decode(1);
      } else {
        // A shift didn't work. Error.
        output.ui32 = 0xFFFFFFFF;
        return output;
      }
    }
    output.ui32 <<= 1;
    output.ui32 |= b0;
  }
  return output;
}

void print_result() {
  const Message result = decode(0);
  Serial.print(result.ui32, HEX);
  Serial.print("\t");
  Serial.print(result.ui32, DEC);
  Serial.print("\t");
  Serial.print(result.i16, DEC);
  Serial.print("\t");
  Serial.print(result.f, DEC);
  Serial.print("\t");
  // End of line.
  Serial.println();
}

void loop() {
  const unsigned long now = micros();
  // Skip until the next message.
  if (0 < state.skip_until) {
    if (now < state.skip_until) return;
    // Reset the skip timer.
    state.skip_until = 0;
    state.prev_clock_micros = now;
  }

  // Read and check the signal.
  const int a5 = analogRead(5);
  const uint16_t sig = (a5 < SIGNAL_LEVEL_THRESHOLD) ? 0 : 1;

  if (state.prev_sig != sig) {
    unsigned long elapsed = now - state.prev_clock_micros;
    // If it spans SYNC_SIGNAL_MICROS, it is a sync message.
    if (elapsed > SYNC_SIGNAL_MICROS) {
      // Skip until the next message.
      state.skip_until = now + (MICROS_PER_CLOCK * CLOCKS_TO_SKIP);
      // Remove the sync signals.
      elapsed -= MICROS_PER_CLOCK * SYNC_SIGNAL_CLOCKS;
      if (elapsed > SINGLE_CLOCKS_MICROS) result_push(state.prev_sig);
      // Print the result.
      print_result();
      result_reset();
    } else {
      // Otherwise, it is a message frame.
      if (elapsed > SINGLE_CLOCKS_MICROS) result_push(state.prev_sig);
      if (elapsed > DOUBLE_CLOCKS_MICROS) result_push(state.prev_sig);
    }
    state.prev_sig = sig;
    state.prev_clock_micros = now;
  }
}
