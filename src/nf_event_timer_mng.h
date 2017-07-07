//
// @Created by CaiZhili on 2017/7/6.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_MNG_H
#define NETFRAMZ_NF_EVENT_TIMER_MNG_H

#include <vector>

#include "nf_event_timer.h"
#include "nf_evnet_timer_heap.h"

class TimerMng {
 public:
  typedef TimerHeap timers_type;
  typedef Timer timer_type;
  
 public:
  int AddTimer(timer_type timer);
  void GetFiredTimers(std::vector<timer_type> &fired_timers);

 private:
  timers_type timers_;
}

#endif //NETFRAMZ_NF_EVENT_TIMER_MNG_H
