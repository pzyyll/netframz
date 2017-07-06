//
// Created by CaiZhili on 2017/6/29.
// Brief
//

#ifndef NETFRAMZ_NF_EVENT_H
#define NETFRAMZ_NF_EVENT_H

//#include <sys/epoll.h>

#include <unordered_map>
#include <queue>
#include <string>

#ifdef LINUX_EPOLL
#include "nf_epoll.h"
#endif

#include "nf_event_task.h"

class EventLoop {
  friend class IOTask;

  typedef std::unordered_map<int, IOTask> TaskMap;
  typedef TaskMap::iterator TaskMapItr;

 public:
  EventLoop();

  void Run();

  int SetIOTask(IOTask &task);
  int DelIOTask(int fd);

  //todo add timer

  const std::string& get_err_msg();
 
 private:
  bool FindTask(const int fd, IOTask &find);
  void set_err_msg(std::string msg);

 private:
  bool stop_;

  TaskMap file_tasks_;

  poll_type poll_;

  std::string err_msg_;
};

#endif //NETFRAMZ_NF_EVENT_H
