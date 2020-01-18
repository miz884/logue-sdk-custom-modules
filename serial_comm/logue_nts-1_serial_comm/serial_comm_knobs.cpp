#include "serial_comm.h"

typedef struct State {
  uint32_t value;
} State;

static State state;

void init_message() {
  state.value = 0;
}

void update_message(const user_osc_param_t *params,
                   int32_t *yn,
                   const uint32_t buf_len) {  
  (void) params;
}

uint32_t get_next_message() {
  return state.value;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  state.value = value;
}
