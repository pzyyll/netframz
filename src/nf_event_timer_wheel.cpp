//
// @Created by CaiZhili on 2017/7/6.
// @bref Timer implement by timing wheel
//

#include "nf_event_timer_wheel.h"

#define NFTVN_BITS (6)
#define NFTVR_BITS (8)
#define NFTVN_SIZE (1 << NFTVN_BITS)
#define NFTVR_SIZE (1 << NFTVR_BITS)
#define NFTVN_MASK (NFTVN_SIZE - 1)
#define NFTVR_MASK (NFTVR_SIZE - 1)
#define LV_SHIFT(x) (NFTVR_BITS + (x)*NFTVN_BITS)

void TimerWheel::_InnerAddTimer(const tnode_type &tnode) {
  timer_list_t *vec = NULL;

  if ((vec = _GetTimerListVec(tnode.expires)) != NULL)
    vec->push_back(tnode);
}

timer_list_t* _GetTimerListVec(const unsigned long expires) {
  unsigned long delta = expires - base_.clk;
  timer_list_t *vec = NULL;

  if (delta < (1 << LV_SHIFT(0))) {
    unsigned idx = expires & NFTVR_MASK;
    vec = base_.tv1.vec + idx;
  } else if (delta < (1 << LV_SHIFT(1))) {
    unsigned idx = (delta >> LV_SHIFT(0)) & NFTVN_MASK;
    vec = base_.tv2.vec + idx;
  } else if (delta < (1 << LV_SHIFT(2))) {
    unsigned idx = (delta >> LV_SHIFT(1)) & NFTVN_MASK;
    vec = base_.tv3.vec + idx;
  } else if (delta < (1 << LV_SHIFT(3))) {
    unsigned idx = (delta >> LV_SHIFT(2)) & NFTVN_MASK;
    vec = base_.tv4.vec + idx;
  } else if ((long)delta < 0) {
    vec = base_.tv1.vec + (base_.clk) & NFTVR_MASK;
  } else {
    if (delta > 0xffffffffUL) {
      delta = 0xffffffffUL;
      expires = delta + base_.clk;
    }

    unsigned idx = (expires >> LV_SHIFT(3)) & NFTVN_MASK;
    vec = base_.tv5.vec + idx;
  }

  return vec;
}
