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
  IOTask();
  IOTask(int fd, int mask);
  IOTask(int fd, int mask, Handle op);

  template <typename Func, typename Obj>
  void Bind(Func &&func, Obj &&obj);
  void Bind(Handle handle);

  void set_fd(int fd);
  int get_fd();

  void set_mask(int mask);
  int get_mask();

  void set_data(task_data_t data);
  task_data_t get_data();

  void Start(EventLoop &loop);
  void Stop(EventLoop &loop);

 protected:
  void Process(EventLoop &loop, IOTask &io_task, int mask);

 private:
  int fd_;
  int mask_;
  Handle op_;
  task_data_t data_;
};

template<typename Func, typename Obj>
void IOTask::Bind(Func &&func, Obj &&obj) {
  op_ = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

#endif //NETFRAMZ_NF_EVENT_TASK_H
