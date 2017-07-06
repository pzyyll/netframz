//
// @Created by CaiZhili on 2017/6/30.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TASK_H
#define NETFRAMZ_NF_EVENT_TASK_H

#include <memory>
#include <functional>
#include "nf_event_config.h"

class EventLoop;

class IOTask;

typedef std::function<void (EventLoop&, IOTask&, int)> FileRwCb;

class IOTask {
  friend class EventLoop;

 public:
  typedef FileRwCb Handle;

 public:
  IOTask() : fd_(-1), mask_(0) { }
  IOTask(int fd, int mask) : fd_(fd), mask_(mask) { }
  IOTask(int fd, int mask, Handle op) : fd_(fd), mask_(mask), op_(op) { }

  template <typename Func, typename Obj>
  void Bind(Func &&func, Obj &&obj);
  void Bind(Handle handle);

  void set_fd(int fd) { fd_ = fd; }
  int get_fd() { return fd_; }

  void set_mask(int mask) { mask_ = mask; }
  int get_mask() { return mask_; }
  
  Handle get_cb() { return op_; }

  void Start(EventLoop &loop);
  void Stop(EventLoop &loop);

 protected:
  void Process(EventLoop &loop, IOTask &io_task, int mask);

 private:
  int fd_;
  int mask_;
  Handle op_;
};

template<typename Func, typename Obj>
void IOTask::Bind(Func &&func, Obj &&obj) {
  op_ = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

#endif //NETFRAMZ_NF_EVENT_TASK_H
