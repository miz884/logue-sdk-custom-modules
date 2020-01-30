#include "serial_comm.h"

typedef struct State {
  Message message;
} State;

static State state;

void init_message() {
  state.message.ui32 = 0L;
}

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len) {  
  (void) params;
}

Message get_next_message() {
  return state.message;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  state.message.ui16 = value;
}
