//
// @Created by CaiZhili on 2017/7/12.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_TASK_H
#define NETFRAMZ_NF_EVENT_TIMER_TASK_H

class Timer;

class EventLoop;

class TimerTask {
  typedef impl_type Timer;
 public:

 private:
  EventLoop &loop_;
  impl_type impl_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_TASK_H
