//
// Created by CaiZhili on 2017/6/29.
//

#ifndef NETFRAMZ_NF_EPOLL_H
#define NETFRAMZ_NF_EPOLL_H

#include <sys/epoll.h>

class nf_epoll {
  typedef struct epoll_event* ep_evs_type;
 public:

 private:
  int m_epfd;
  int m_timeout;
  ep_evs_type m_evs;
  size_t m_maxevs;
};

#endif //NETFRAMZ_NF_EPOLL_H
