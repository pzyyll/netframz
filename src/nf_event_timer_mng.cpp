//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#include "nf_event_timer_mng.h"

TimerMng::TimerMng() {
    timers_.Init();
}

int TimerMng::AddTimer(timer_type timer) {
  struct timeval expire = timer.GetExpireTime();
  return timers_.AddTimer(expire, timer);
}

void TimerMng::GetFiredTimers(std::vector<timer_type> &fired_timers) {
  timers_.GetTimer(fired_timers);
}
unsigned long TimerMng::GetTimerSize() {
  return timers_.GetTimerSize();
}


