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

#define EV_POLLIN EPOLLIN
#define EV_POLLOUT EPOLLOUT

struct FiredEvent {
    int id;
    int mask;
};

class Epoll {
public:
    typedef struct epoll_event ev_type;
    typedef struct epoll_event *ev_pointer;
    typedef unsigned int data_type;

    const static int DEFAULT_MAXEVS = 1024;
    const static int DEFAULT_TIMEOUT = 10;

public:
    Epoll();

    ~Epoll();

    int Init(const int timeout = DEFAULT_TIMEOUT, const int maxevs = DEFAULT_MAXEVS);

    void Deinit();

    int AddEvent(int fd, data_type data, int mask);

    int DelEvent(int fd);

    int ModEvent(int fd, data_type data, int mask);

    int WaitEvent(std::deque<FiredEvent> &fires);

    int Resize(const size_t maxevs);

    std::string get_err();

private:
    Epoll(const Epoll &);

    Epoll &operator=(const Epoll &);

private:
    int CtlEvent(int fd, int op, data_type data, int mask);

    void set_err(const char *s, ...);

private:
    int epfd_;
    int timeout_;
    int maxevs_;
    ev_pointer evs_;
    char err_[256];
};

typedef Epoll poll_type;

#endif //NETFRAMZ_NF_EPOLL_H
