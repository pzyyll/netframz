//
// @Created by CaiZhili on 2017/7/6.
// @bref Timer implement by timing wheel
//

#include "nf_event_timer_wheel.h"

#define LV_SHIFT(x) (NFTVR_BITS + (x)*NFTVN_BITS)

void TimerWheel::Init() {
  base_.size = 0;
  base_.clk = _GetDeafaultJiffers();
}

int TimerWheel::AddTimer(const struct timeval &expire, const timer_type &timer_data) {
  unsigned long expires_clk = _GetJiffersFromTime(expire);
  timer_node_t tnode = {expires_clk, timer_data};

  _InnerAddTimer(tnode);

  return 0;
}

int TimerWheel::GetTimer(timer_vec_type &all_timer) {
  return _TimerRun(all_timer, _GetDeafaultJiffers());
}

unsigned long TimerWheel::GetTimerSize() {
  return base_.size;
}

void TimerWheel::_InnerAddTimer(const timer_node_t &tnode) {
  timer_list_t *vec = NULL;

  if ((vec = _GetTimerListVec(tnode.expires)) != NULL) {
    vec->push_back(tnode);
    ++base_.size;
  }
}

timer_list_t* TimerWheel::_GetTimerListVec(const unsigned long expires) {
  unsigned long expires_clk = expires;
  unsigned long delta = expires_clk - base_.clk;
  timer_list_t *vec = NULL;

  if (delta < (1 << LV_SHIFT(0))) {
    unsigned long idx = _IndexOfTvn(expires_clk, 0);    //expires_clk & NFTVR_MASK;
    vec = base_.tv.vec + idx;
  } else if (delta < (1 << LV_SHIFT(1))) {
    unsigned long idx = _IndexOfTvn(expires_clk, 1);    //(expires_clk >> LV_SHIFT(0)) & NFTVN_MASK;
    vec = base_.tv1.vec + idx;
  } else if (delta < (1 << LV_SHIFT(2))) {
    unsigned long idx = _IndexOfTvn(expires_clk, 2);    //(expires_clk >> LV_SHIFT(1)) & NFTVN_MASK;
    vec = base_.tv2.vec + idx;
  } else if (delta < (1 << LV_SHIFT(3))) {
    unsigned long idx = _IndexOfTvn(expires_clk, 3);    //(expires_clk >> LV_SHIFT(2)) & NFTVN_MASK;
    vec = base_.tv3.vec + idx;
  } else if ((long)delta < 0) {
    vec = base_.tv.vec + _IndexOfTvn(base_.clk, 0);     //(base_.clk & NFTVR_MASK);
  } else {
    if (delta > 0xffffffffUL) {
      delta = 0xffffffffUL;
      expires_clk = delta + base_.clk;
    }

    unsigned long idx = _IndexOfTvn(expires_clk, 4);
    vec = base_.tv4.vec + idx;
  }

  return vec;
}

int TimerWheel::_TimerRun(timer_vec_type &all_timer, unsigned long jiffers) {

  while ((long)jiffers >= (long)base_.clk) {
    timer_list_t work_list;
    unsigned int tvr_idx = _IndexOfTvn(base_.clk, 0);
    //todo
    if (!tvr_idx &&
        !_Cascade(&base_.tv1, _IndexOfTvn(base_.clk, 1)) &&
        !_Cascade(&base_.tv2, _IndexOfTvn(base_.clk, 2)) &&
        !_Cascade(&base_.tv3, _IndexOfTvn(base_.clk, 3)))
      _Cascade(&base_.tv4, _IndexOfTvn(base_.clk, 4));

    ++base_.clk;

    timer_list_t &curr_tick_list = base_.tv.vec[tvr_idx];
    work_list.splice(work_list.begin(), curr_tick_list);
    base_.size -= work_list.size();
    for (timer_list_t::iterator itr = work_list.begin();
         itr != work_list.end();
         ++itr) {
      all_timer.push_back(itr->timer);
    }
  }

  return all_timer.size();
}

int TimerWheel::_Cascade(nftv_t *tv, int index) {
  //当节拍走完当前层的一轮时间后重新搬迁下一层中 tick 链中的定时器。
  timer_list_t &tv_list = tv->vec[index];
  for (timer_list_t::iterator itr = tv_list.begin(); itr != tv_list.end(); ++itr) {
    _InnerAddTimer(*itr);
  }
  //统一清除搬迁后的定时器链。
  tv_list.clear();

  return index;
}

static inline unsigned long TimerWheel::_IndexOfTvn(unsigned long clk, unsigned int n) {
  if (n <= 0) {
    return (clk & NFTVR_MASK);
  } else if (n <= 4) {
    return ((clk >> LV_SHIFT(n - 1)) & NFTVN_MASK);
  }

  return (unsigned long)0-1;
}

static inline unsigned long TimerWheel::_GetJiffersFromTime(const struct timeval &t)
{
  return (t.tv_sec * (unsigned long)(1e6 / PRECISION_USEC)
      + t.tv_usec / (unsigned long)PRECISION_USEC);
}

static inline unsigned long TimerWheel::_GetDeafaultJiffers() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return _GetJiffersFromTime(now);
}