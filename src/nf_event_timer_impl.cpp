//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#include <sys/time.h>
#include "nf_event.h"
#include "nf_event_timer_impl.h"

TimerImpl::TimerImpl(const unsigned long interval, bool is_loop)
    :id_(0), interval_(interval), is_loop_(is_loop) {

}

TimerImpl::~TimerImpl() {

}

void TimerImpl::Bind(handle_t handle) {
  op_ = handle;
}

struct timeval TimerImpl::GetExpireTime() {
  struct timeval expire;
  expire.tv_sec = begin_.tv_sec;
  expire.tv_usec = begin_.tv_usec + interval_ * 1000;
  if (expire.tv_usec > (1000000 - 1)) {
    expire.tv_sec += expire.tv_usec / 1000000;
    expire.tv_usec = expire.tv_usec % 1000000;
  }

  return expire;
}

void TimerImpl::Process(EventLoop *loop, int mask) {
  if (op_)
    op_(loop, data_, mask);
}
