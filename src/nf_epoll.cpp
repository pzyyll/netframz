//
// Created by CaiZhili on 2017/6/29.
//

#include <cstring>
#include <unistd.h>

#include "nf_epoll.h"

Epoll::Epoll()
    : m_epfd(-1),
      m_timeout(DEFAULT_TIMEOUT),
      m_evs(NULL),
      m_maxevs(DEFAULT_MAXEVS) {

}

Epoll::~Epoll() {
  if (m_epfd > 0) {
    close(m_epfd);
  }
  if (m_evs != NULL) {
    free(m_evs);
  }
  m_epfd = -1;
  m_evs = NULL;
}

int Epoll::Init(const int timeout, const size_t maxevs) {
  m_timeout = timeout;

  m_evs = static_cast<ev_pointer >(malloc(maxevs * sizeof(ev_type)));
  if (NULL == m_evs) {
    set_err("Alloc memory to events array fail.");
    return RET::RET_FAIL;
  }
  m_maxevs = maxevs;

  m_epfd = epoll_create1(EPOLL_CLOEXEC);
  if (m_epfd < 0) {
    set_err("Try to create epoll fd fail|err=%s", strerror(errno));
    return RET::RET_FAIL;
  }

  return RET::RET_SUCCESS;
}

int Epoll::AddEvent(int fd, Epoll::data_type data, int mask) {
  return CtlEvent(fd, EPOLL_CTL_ADD, data, mask);
}

int Epoll::DelEvent(int fd) {
  return CtlEvent(fd, EPOLL_CTL_DEL, 0, 0);
}

int Epoll::ModEvent(int fd, Epoll::data_type data, int mask) {
  return CtlEvent(fd, EPOLL_CTL_MOD, data, mask);
}

int Epoll::WaitEvent(std::vector<FiredTask> &fires) {
  int nfds = 0;
  nfds = epoll_wait(m_epfd, m_evs, static_cast<int>(m_maxevs), m_timeout);
  if (nfds > 0) {
    for (int i = 0; i < nfds; ++i) {
      FiredTask task = {0, 0};
      ev_pointer pev = m_evs + i;
      task.id = pev->data.u32;
      if (pev->events & EPOLLIN) task.mask |= EVSTAT::EV_READABLE;
      if (pev->events & EPOLLOUT) task.mask |= EVSTAT::EV_WRITEABLE;
      if (pev->events & EPOLLHUP) task.mask |= EVSTAT::EV_HUP;
      if (pev->events & EPOLLRDHUP) task.mask |= EVSTAT::EV_RDHUP;
      if (pev->events & EPOLLERR) task.mask |= EVSTAT::EV_ERR;

      fires.push_back(task);
    }
  }

  return RET::RET_SUCCESS;
}

int Epoll::Resize(const size_t maxevs) {
  if (maxevs > 0) {
    ev_pointer pevs = static_cast<ev_pointer >(realloc(m_evs, maxevs));
    if (pevs) {
      m_evs = pevs;
      m_maxevs = maxevs;
    }
  }
  return 0;
}

std::string Epoll::get_err() {
  return std::string(m_err);
}

void Epoll::set_err(const char *s, ...) {
  va_list args;
  va_start(args, s);
  vsnprintf(m_err, sizeof(m_err), s, args);
  va_end(args);
}

int Epoll::CtlEvent(int fd, int op, Epoll::data_type data, int mask) {
  switch (op) {
    case EPOLL_CTL_DEL: {
      if (epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        set_err("Epoll del event fail|%s", strerror(errno));
        return RET::RET_FAIL;
      }
      break;
    }
    case EPOLL_CTL_ADD:
    case EPOLL_CTL_MOD: {
      ev_type ev;
      ev.data.u32 = data;
      ev.events |= mask | EPOLLET | EPOLLRDHUP;

      if (epoll_ctl(m_epfd, op, fd, &ev) < 0) {
        set_err("Epoll ctl op(%d) fail|%s", op, strerror(errno));
        return RET::RET_FAIL;
      }
      break;
    }
    default:
      set_err("Epoll op(%d) is invaild.");
      return RET::RET_FAIL;
  }
  return RET::RET_SUCCESS;
}

