//
// @Created by CaiZhili on 2017/7/6.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_MNG_H
#define NETFRAMZ_NF_EVENT_TIMER_MNG_H

#include <vector>

#include "nf_event_timer.h"

#ifdef NF_TIMER_WHEEL
#include "nf_event_timer_wheel.h"
typedef TimerWheel timers_type;
#else 
#include "nf_event_timer_heap.h"
typedef TimerHeap timers_type;
#endif //NF_TIMER_WHEEL

class TimerMng {
 public:
  typedef Timer timer_type;
  
 public:
  int AddTimer(timer_type timer);
  void GetFiredTimers(std::vector<timer_type> &fired_timers);
  unsigned long GetTimerSize();
 private:
  timers_type timers_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_MNG_H
