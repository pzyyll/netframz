//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_H_
#define NETFRAMZ_NF_EVENT_TIMER_H_

#include <functional>
#include <sys/time.h>
#include "nf_event_config.h"
#include "impl/nf_operator.h"

class EventLoop;

class Timer;

class Timer : public Operator {
  friend class TimerMng;
  friend class EventLoop;
 public:
  typedef unsigned long long id_type;
 public:
  Timer();
  Timer(const unsigned long expire, bool loop);

  void Start(EventLoop &loop);
  void Restart(EventLoop &loop);
  void Stop(EventLoop &loop);
  void StopLoop();

  struct timeval GetExpireTime();

  id_type get_id() { return id_; }

  void set_begin(struct timeval begin) { begin_ = begin; }

  bool get_is_loop() { return is_loop_; };

 protected:
  void set_id(id_type id) { id_ = id; }
  //void Process(EventLoop &loop, Timer &timer, int mask);

 private:
  //default = 0
  id_type id_;

  struct timeval begin_;

  //ms, expire time = begin_ + interval_;
  unsigned long interval_;

  bool is_loop_;
  task_data_t data_;
};


#endif //NETFRAMZ_NF_EVENT_TIMER_IMPLV1_H_

