//
// Created by CaiZhili on 2017/6/29.
// Brief
//

#ifndef NETFRAMZ_NF_EVENT_H
#define NETFRAMZ_NF_EVENT_H

#include <map>
#include <list>
#include <vector>
#include <queue>
#include <string>

#ifdef LINUX_EPOLL

#include "nf_epoll.h"

#endif

#include "nf_event_timer_mng.h"

class IOTaskImpl;

class IdleImpl;

class EventLoop {
public:
    typedef IOTaskImpl *iotask_pointer;
    typedef TimerImpl *timer_pointer;
    typedef IdleImpl *idle_pointer;
    typedef std::map<unsigned long, iotask_pointer> TaskMap;
    typedef TaskMap::iterator TaskMapItr;
    typedef std::list<idle_pointer> IdleList;

public:
    EventLoop();

    ~EventLoop();

    int Init();

    void Run();

    void Stop();

    void Sleep(int timeout);


    int SetIOTask(iotask_pointer task);

    int ResetIOTask(iotask_pointer task);

    int DelIOTask(int fd);

    int AddTimerTask(timer_pointer timer);

    int DelTimerTask(const unsigned long id);

    int ResetTimerTask(timer_pointer timer);

    int AddIdleTask(idle_pointer idler);

    int DelIdleTask(idle_pointer idler);

    const std::string &get_err_msg();

private:
    void PollTask(int timeout);

    bool FindTask(const int fd, iotask_pointer *find);

    void HandleAllIOTask();

    void HandleAllTimerTask();

    void HandleAllIdleTask();

    void set_err_msg(std::string msg);

private:
    bool                   stop_;
    int                    waittime_;
    poll_type              poll_;
    TimerMng               timer_mng_;
    TaskMap                file_tasks_;
    std::deque<FiredEvent> fires_que_;
    IdleList               idle_tasks_;
    std::string            err_msg_;
};

#endif //NETFRAMZ_NF_EVENT_H
