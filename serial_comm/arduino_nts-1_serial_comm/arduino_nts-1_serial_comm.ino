#define param_val_to_f32(val) ((uint16_t)val * 9.77517106549365e-004f)  

#define SIGNAL_LEVEL_THRESHOLD 50

#define SINGLE_CLOCK_LEN 90
#define DOUBLE_CLOCK_LEN 180
#define STOP_CLOCK_LEN 360

typedef struct State {
  uint16_t run_length = 0;
  int8_t   prev_sig = 0;
  uint8_t  sig_clock = 0;

  unsigned long skip_until = 0;

  String result_str = "";
} State;
static State state;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
  
  state.run_length = 0;
  state.prev_sig = 0;
  state.sig_clock = 0;

  state.skip_until = 0;

  state.result_str = "";
}

void put_signal(int8_t sig, uint16_t len) {
  if (len > DOUBLE_CLOCK_LEN) state.result_str.concat(sig);
  if (len > SINGLE_CLOCK_LEN)  state.result_str.concat(sig);
}

String decode(String s, int p) {
  String output = "";
  for (int i = p; i < s.length(); i += 2) {
    char c0 = s.charAt(i);
    char c1 = s.charAt(i + 1);
    // Same signal can't appear in 2 frames in a row.
    if (c0 == c1) {
      if (p == 0) {
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
  uint32_t result_i = 0;
  for (int i = 0; i < result_s.length(); ++i) {
    result_i <<= 1;
    if (result_s.charAt(i) == '1') result_i |= 1;
  }
  float result_f = param_val_to_f32(result_i);
  Serial.print(result_s.substring( 0, 8));
  Serial.print(" ");
  Serial.print(result_s.substring(8, 16));
  Serial.print(" ");
  Serial.print(result_s.substring(16, 24));
  Serial.print(" ");
  Serial.print(result_s.substring(24));
  Serial.print("\t");
  Serial.print(result_i);
  Serial.print("\t");
  Serial.print(result_f);
  Serial.println();
}

void loop() {
  // Skip until the next message frames.
  if (state.skip_until > 0 && micros() < state.skip_until) return;
  state.skip_until = 0;

  // Read and check the signal.
  int a5 = analogRead(5);
  int8_t sig = -1;
  if (a5 < SIGNAL_LEVEL_THRESHOLD) sig = 0;
  else if (SIGNAL_LEVEL_THRESHOLD <= a5) sig = 1;

  if (state.prev_sig == sig) {
    ++state.run_length;
  } else {
    // If it spans STOP_CLOCK_LEN, it is the end of message.
    if (state.run_length > STOP_CLOCK_LEN) {
      // Skip until the next message frame.
      // There are 80 clocks in a second. There are 10 clocks to skip.
      state.skip_until = micros() + (1000L * 1000L / 80 * 10);
      // Remove the unused frames at the end of message.
      state.result_str.remove(state.result_str.length() - 2, 2);
      // Print the result.
      print_result(state.result_str);
      state.result_str = "";
    } else {
      // Otherwise, it is a message frame.
      put_signal(state.prev_sig, state.run_length);
    }
    state.prev_sig = sig;
    state.run_length = 0;
  }
}
