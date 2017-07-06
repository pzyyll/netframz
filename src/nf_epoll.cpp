//
// Created by CaiZhili on 2017/6/29.
//

#include <cstring>
#include <cstdarg>
#include <unistd.h>

#include "nf_epoll.h"

Epoll::Epoll()
    : epfd_(-1),
      timeout_(DEFAULT_TIMEOUT),
      maxevs_(DEFAULT_MAXEVS),
      evs_(NULL) {
}

Epoll::Epoll(const Epoll &rhs)
    : epfd_(rhs.epfd_),
      timeout_(rhs.timeout_),
      maxevs_(rhs.maxevs_) {
  ev_pointer new_ptr = static_cast<ev_pointer>(malloc(rhs.maxevs_ * sizeof(ev_type)));
  memcpy(new_ptr, rhs.evs_, rhs.maxevs_);
  evs_ = new_ptr;
  strncpy(err_, rhs.err_, sizeof(err_));
}

Epoll &Epoll::operator=(const Epoll &rhs) {
  epfd_ = rhs.epfd_;
  timeout_ = rhs.timeout_;
  maxevs_ = rhs.maxevs_;
  ev_pointer new_ptr = static_cast<ev_pointer>(malloc(rhs.maxevs_ * sizeof(ev_type)));
  memcpy(new_ptr, rhs.evs_, rhs.maxevs_);
  evs_ = new_ptr;
  strncpy(err_, rhs.err_, sizeof(err_));
  return *this;
}

Epoll::~Epoll() {
  Deinit();
}

int Epoll::Init(const int timeout, const int maxevs) {
  timeout_ = timeout;

  evs_ = static_cast<ev_pointer >(malloc(maxevs * sizeof(ev_type)));
  if (NULL == evs_) {
    set_err("Alloc memory to events array fail.");
    return RET::RET_FAIL;
  }
  maxevs_ = maxevs;

  epfd_ = epoll_create1(EPOLL_CLOEXEC);
  if (epfd_ < 0) {
    set_err("Try to create epoll fd fail|err=%s", strerror(errno));
    return RET::RET_FAIL;
  }

  return RET::RET_SUCCESS;
}

void Epoll::Deinit() {
  if (epfd_ > 0) {
    ::close(epfd_);
  }
  if (evs_ != NULL) {

    free(evs_);
  }
  epfd_ = -1;
  evs_ = NULL;
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

int Epoll::WaitEvent(std::deque<FiredTask> &fires) {
  int nfds = 0;
  nfds = epoll_wait(epfd_, evs_, maxevs_, timeout_);
  if (nfds > 0) {
    for (int i = 0; i < nfds; ++i) {
      FiredTask task = {0, 0};
      ev_pointer pev = evs_ + i;
      task.id = pev->data.u32;
      if (pev->events & EPOLLIN) task.mask |= EVSTAT::EV_READABLE;
      if (pev->events & EPOLLOUT) task.mask |= EVSTAT::EV_WRITEABLE;
      if (pev->events & EPOLLHUP) task.mask |= EVSTAT::EV_HUP;
      if (pev->events & EPOLLRDHUP) task.mask |= EVSTAT::EV_RDHUP;
      if (pev->events & EPOLLERR) task.mask |= EVSTAT::EV_ERR;

      fires.push_back(task);
    }
  }

  return nfds;
}

int Epoll::Resize(const size_t maxevs) {
  if (maxevs > 0) {
    ev_pointer pevs = static_cast<ev_pointer >(realloc(evs_, maxevs));
    if (pevs) {
      evs_ = pevs;
      maxevs_ = maxevs;
    }
  }
  return 0;
}

std::string Epoll::get_err() {
  return std::string(err_);
}

void Epoll::set_err(const char *s, ...) {
  va_list args;
  va_start(args, s);
  vsnprintf(err_, sizeof(err_), s, args);
  va_end(args);
}

int Epoll::CtlEvent(int fd, int op, Epoll::data_type data, int mask) {
  switch (op) {
    case EPOLL_CTL_DEL: {
      if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) < 0) {
        set_err("Epoll del event fail|%s", strerror(errno));
        return RET::RET_FAIL;
      }
      break;
    }
    case EPOLL_CTL_ADD:
    case EPOLL_CTL_MOD: {
      ev_type ev;
      ev.data.u32 = data;
      ev.events = mask | EPOLLET | EPOLLRDHUP;

      if (epoll_ctl(epfd_, op, fd, &ev) < 0) {
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



