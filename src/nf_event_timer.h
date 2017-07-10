//
// @Created by CaiZhili on 2017/7/6.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_H_
#define NETFRAMZ_NF_EVENT_TIMER_H_

#include <functional>
#include <sys/time.h>

class EventLoop;
class Timer;

typedef std::function<void (EventLoop&, Timer&, int)> TimerCb;

class Timer {
  friend class EventLoop;
  typedef TimerCb Handle;
 public:
  typedef unsigned long long id_type;
 public:
  Timer(const unsigned long expire, bool loop);

  template <typename Func, typename Obj>
  void Bind(Func &&func, Obj &&obj);
  void Bind(Handle handle);

  void Start(EventLoop &loop);
  void StopLoop(); 

  struct timeval GetExpireTime();

  void set_id(id_type id) { id_ = id; }
  id_type get_id() { return id_; }

  void set_begin(struct timeval begin) { begin_ = begin; }

  bool get_is_loop() { return is_loop_; };
  
 protected:
  void Process(EventLoop &loop, Timer &timer, int mask);
 
 private:
  //default = 0
  id_type id_;
  
  struct timeval begin_;
  
  //ms, expire time = begin_ + interval_;
  unsigned long interval_;
  
  bool is_loop_;
  Handle op_;
  task_data_t data_;
};

template<typename Func, typename Obj>
void Timer::Bind(Func &&func, Obj &&obj) {
  op_ = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}


#endif //NETFRAMZ_NF_EVENT_TIMER_IMPLV1_H_

