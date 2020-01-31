#include "userdelfx.h"


void DELFX_INIT(uint32_t platform, uint32_t api) {
  (void) platform;
}

void DELFX_PROCESS(float *xn, uint32_t buf_len) {
  float * __restrict x = xn;
  const float * x_e = x + 2 * buf_len;
  while (x != x_e) {
    *(x++) = *(x++) = 0.f;
  }
}
