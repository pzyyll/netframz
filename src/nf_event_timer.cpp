//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#include <sys/time.h>
#include "nf_event.h"
#include "nf_event_timer.h"

Timer::Timer(EventLoop &loop, const unsigned long expire, bool is_loop)
    : loop_(loop), id_(0), interval_(expire), is_loop_(is_loop) {
  gettimeofday(&begin_, NULL);
}

Timer::~Timer() {
  Stop();
}

void Timer::Bind(Handle handle) {
  op_ = handle;
}

void Timer::Start() {
  loop_.AddTimerTask(this);
}

void Timer::Restart() {
  loop_.ResetTimerTask(this);
}

void Timer::Stop() {
  loop_.DelTimerTask(id_);
}

struct timeval Timer::GetExpireTime() {
  struct timeval expire;
  expire.tv_sec = begin_.tv_sec;
  expire.tv_usec = begin_.tv_usec + interval_ * 1000;
  if (expire.tv_usec > (1000000 - 1)) {
    expire.tv_sec += expire.tv_usec / 1000000;
    expire.tv_usec = expire.tv_usec % 1000000;
  }

  return expire;
}

void Timer::Process(EventLoop &loop, Timer &timer, int mask) {
  if (op_)
    op_(loop, timer, mask);
}
