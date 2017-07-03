//
// Created by CaiZhili on 2017/6/29.
// Brief
//

#ifndef NETFRAMZ_NF_EVENT_H
#define NETFRAMZ_NF_EVENT_H

//#include <sys/epoll.h>

#include <unordered_map>
#include <queue>

#define LINUX_EPOLL
#ifdef LINUX_EPOLL
#include "nf_epoll.h"
#endif

#include "nf_event_task.h"

class EventLoop {
  friend class IOTask;

  typedef std::unordered_map<int, TaskPtr> TaskMap;

 public:
  void Run();

  int SetIOTask(IOTask &task);
  int DelIOTask(IOTask &task);

 private:
  template <typename T>
  std::shared_ptr<T> FindTask(T &task_map);

 private:
  bool stop_;

  TaskMap file_tasks_;

  poll_type poll_;
};

#endif //NETFRAMZ_NF_EVENT_H
