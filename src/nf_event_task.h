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


class IOTask {
  friend class EventLoop;

 public:
  typedef std::function<void (EventLoop&, IOTask&, int)> handle_t;

 public:
  IOTask(EventLoop &loop);
  IOTask(EventLoop &loop, const int fd, const int mask);
  IOTask(EventLoop &loop, const int fd, const int mask, handle_t op);
  ~IOTask();

  void Bind(handle_t handle);
  void Start();
  void Restart();
  void Stop();

 public:
  void set_fd(int fd);
  int get_fd();

  void set_mask(int mask);
  int get_mask();

  void set_data(task_data_t data);
  task_data_t get_data();

 protected:
  void Process(EventLoop &loop, IOTask &io_task, int mask);

 private:
  EventLoop &loop_;
  int fd_;
  int mask_;
  handle_t op_;
  task_data_t data_;
};

#endif //NETFRAMZ_NF_EVENT_TASK_H
