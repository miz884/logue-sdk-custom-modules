#include "usermodfx.h"

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void) platform;
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn,  float *sub_yn,
                   uint32_t buf_len) {
  const float *mx = main_xn;
  float * __restrict my = main_yn;
  const float * my_e = my + 2 * buf_len;

  const float *sx = sub_xn;
  float * __restrict sy = sub_yn;

  while (my != my_e) {
    *(my++) = *(mx++);
    *(my++) = *(mx++);
    *(sy++) = *(sx++);
    *(sy++) = *(sx++);
  }

}
