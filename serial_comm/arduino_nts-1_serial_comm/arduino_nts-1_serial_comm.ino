#define param_val_to_f32(val) ((uint16_t) val * 9.77517106549365e-004f)  

#define SIGNAL_LEVEL_THRESHOLD (50)

#define FRAMES_PER_CLOCK (80.f)

#define MICROS_PER_FRAME (1.f / 48000.f * 1000.f * 1000.f)
#define MICROS_PER_CLOCK (MICROS_PER_FRAME * FRAMES_PER_CLOCK)
#define ACCEPTABLE_ERROR (0.90f)

#define SINGLE_CLOCKS_MICROS (MICROS_PER_CLOCK * 1.f * ACCEPTABLE_ERROR)
#define DOUBLE_CLOCKS_MICROS (MICROS_PER_CLOCK * 2.f * ACCEPTABLE_ERROR)
#define STOP_SIGNAL_MICROS (MICROS_PER_CLOCK * 3.f * ACCEPTABLE_ERROR)

typedef struct State {
  int8_t prev_sig = 0;
  unsigned long prev_clock_micros = 0;
  unsigned long skip_until = 0;

  String result_str = "";
} State;
static State state;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
  
  state.prev_sig = 0;
  state.prev_clock_micros = 0;
  state.skip_until = 0;
  state.result_str = "";
}

/*
 * Decode a Manchester code.
 */
String decode(String s, int offset) {
  String output = "";
  for (int i = offset; i < s.length(); i += 2) {
    char c0 = s.charAt(i);
    char c1 = s.charAt(i + 1);
    // Same signal can't appear in 2 frames in a row.
    if (c0 == c1) {
      if (offset == 0) {
        // Try to shift a frame.
        return decode(s, 1);
      } else {
        // A shift didn't work. Error.
        return "Error: decode failed.";
      }
    }
    output.concat(c0);
  }
  return output;
}

void print_result(String s) {
  String result_s = decode(s, 0);
  Serial.print(result_s.substring( 0, 8));
  Serial.print(" ");
  Serial.print(result_s.substring(8, 16));
  Serial.print(" ");
  Serial.print(result_s.substring(16, 24));
  Serial.print(" ");
  Serial.print(result_s.substring(24));
  // Convert it into uint32.
  uint32_t result_ui = 0;
  for (int i = 0; i < result_s.length(); ++i) {
    result_ui <<= 1;
    if (result_s.charAt(i) == '1') result_ui |= 1;
  }
  Serial.print("\t");
  Serial.print(result_ui);
  // Convert it into signed int.
  int32_t result_i = (int16_t) result_ui;
  Serial.print("\t");
  Serial.print(result_i);
  // Convert it into a floating point value.
  float result_f = param_val_to_f32(result_i);
  Serial.print("\t");
  Serial.print(result_f);
  // End of line.
  Serial.println();
}

void loop() {
  unsigned long now = micros();
  // Skip until the next message.
  if (state.skip_until > 0 && now < state.skip_until) return;

  if (state.skip_until > 0) {
    state.skip_until = 0;
    state.prev_clock_micros = now;
  }

  // Read and check the signal.
  const int a5 = analogRead(5);
  const int8_t sig = (a5 < SIGNAL_LEVEL_THRESHOLD) ? 0 : 1;

  if (state.prev_sig != sig) {
    const unsigned long elapsed = now - state.prev_clock_micros;
    // If it spans STOP_SIGNAL_MICROS, it is the end of message.
    if (elapsed > STOP_SIGNAL_MICROS) {
      // Skip until the next message (3 clocks to skip).
      state.skip_until = now + (MICROS_PER_CLOCK * 3);
      // Remove the unused frames at the end of message.
      if (state.result_str.length() > 2) {
        state.result_str.remove(state.result_str.length() - 2, 2);
      }
      // Print the result.
      print_result(state.result_str);
      state.result_str = "";
    } else {
      // Otherwise, it is a message frame.
      if (elapsed > SINGLE_CLOCKS_MICROS) state.result_str.concat(state.prev_sig);
      if (elapsed > DOUBLE_CLOCKS_MICROS) state.result_str.concat(state.prev_sig);
    }
    state.prev_sig = sig;
    state.prev_clock_micros = now;
  }
}
