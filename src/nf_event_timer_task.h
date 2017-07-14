//
// @Created by CaiZhili on 2017/7/12.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_TASK_H
#define NETFRAMZ_NF_EVENT_TIMER_TASK_H

#include <functional>
#include "nf_event_config.h"

class TimerImpl;

class EventLoop;

class TimerTask {
  typedef TimerImpl impl_type;
 public:
  typedef std::function<void (EventLoop&, TimerTask&, int)> handle_t;
 public:
  TimerTask(EventLoop &loop, const unsigned long expire, bool is_loop);
  ~TimerTask();

  void Bind(handle_t handle);
  void Start();
  void Restart();
  void Stop();

  void SetPrivateData(task_data_t data);
  task_data_t GetPrivateData();

 private:
  EventLoop &loop_;
  impl_type *impl_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_TASK_H
