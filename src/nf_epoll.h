//
// Created by CaiZhili on 2017/6/29.
//

#ifndef NETFRAMZ_NF_EPOLL_H
#define NETFRAMZ_NF_EPOLL_H

#include <sys/epoll.h>
#include <cstddef>
#include <string>
#include <queue>
#include "nf_event_config.h"
#include "nf_event_task.h"

#define EV_POLLIN EPOLLIN
#define EV_POLLOUT EPOLLOUT

class Epoll {
 public:
  typedef struct epoll_event ev_type;
  typedef struct epoll_event* ev_pointer;
  typedef unsigned int data_type;

  const size_t DEFAULT_MAXEVS = 1024;
  const int DEFAULT_TIMEOUT = 10;

 public:
  Epoll();
  ~Epoll();

  int Init(const int timeout = DEFAULT_TIMEOUT, const size_t maxevs = DEFAULT_MAXEVS);
  int AddEvent(int fd, data_type data, int mask);
  int DelEvent(int fd);
  int ModEvent(int fd, data_type data, int mask);
  int WaitEvent(std::deque<FiredTask> &fires);
  int Resize(const size_t maxevs);

  std::string get_err();

 private:
  int CtlEvent(int fd, int op, data_type data, int mask);
  void set_err(const char *s, ...);

 private:
  int m_epfd;
  int m_timeout;
  size_t m_maxevs;
  ev_pointer m_evs;
  char m_err[256];
};

typedef Epoll poll_type;

#endif //NETFRAMZ_NF_EPOLL_H
