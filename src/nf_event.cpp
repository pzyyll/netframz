//
// Created by CaiZhili on 2017/6/29.
//

#include <type_traits>

#include "nf_event.h"
#include "nf_event_timer_impl.h"
#include "nf_event_iotask_impl.h"
#include "nf_event_idle_impl.h"

static const int kMaxBlockTime = 10;

EventLoop::EventLoop() : stop_(true), waittime_(0) {
    poll_.Init();
}

EventLoop::~EventLoop() {

}

int EventLoop::Init() {
    int ret = poll_.Init();
    if (ret < 0) {
        set_err_msg(poll_.get_err());
        return ret;
    }
    stop_ = false;
    return ret;
}

void EventLoop::Run() {
    bool exist_task = true;

    while (!stop_ && exist_task) {
        PollTask(waittime_);

        HandleAllTimerTask();

        HandleAllIOTask();

        HandleAllIdleTask();

        //Check if exist task continue loop.
        exist_task = false;
        if (idle_tasks_.size() > 0) {
            exist_task = true;
            waittime_ = 0;
        } else {
            waittime_ = kMaxBlockTime;
        }
        if (timer_mng_.GetTimerSize() > 0 ||
            file_tasks_.size() > 0) {
            exist_task = true;
        }
    }
}

void EventLoop::Stop() {
    stop_ = true;
}

void EventLoop::Sleep(int timeout) {
    PollTask(timeout);
}

int EventLoop::SetIOTask(iotask_pointer task) {
    int ret = RET::RET_SUCCESS;

    if (NULL == task) {
        set_err_msg("task ptr is null");
        return RET_FAIL;
    }

    if (FindTask(task->get_fd(), NULL)) {
        set_err_msg("task is exit.");
        return RET_FAIL;
    }

    ret = poll_.AddEvent(task->get_fd(), task->get_fd(), task->get_mask());
    if (ret != RET::RET_SUCCESS) {
        set_err_msg(poll_.get_err());
        return ret;
    }
    file_tasks_[task->get_fd()] = task;

    return ret;
}

int EventLoop::ResetIOTask(iotask_pointer task) {
    int ret = RET_SUCCESS;

    if (NULL == task) {
        set_err_msg("ResetIOTask: task ptr is null.");
        return RET_FAIL;
    }

    int fd = task->get_fd();
    if (!FindTask(fd, NULL)) {
        set_err_msg("ResetIOTask: task is not exit.");
        return RET_FAIL;
    }

    ret = poll_.ModEvent(fd, fd, task->get_mask());
    if (ret != RET::RET_SUCCESS) {
        set_err_msg(poll_.get_err());
        return ret;
    }
    file_tasks_[fd] = task;

    return ret;
}

int EventLoop::DelIOTask(int fd) {
    int ret = RET::RET_FAIL;

    if (FindTask(fd, NULL)) {
        ret = poll_.DelEvent(fd);
        if (ret != RET::RET_SUCCESS) {
            set_err_msg(poll_.get_err());
            return ret;
        }
        file_tasks_.erase(fd);
        return RET::RET_SUCCESS;
    }

    set_err_msg("Not Find Task!");
    return ret;
}

int EventLoop::AddTimerTask(timer_pointer timer) {
    return timer_mng_.AddTimer(timer);
}

int EventLoop::DelTimerTask(const unsigned long id) {
    return timer_mng_.DelTimer(id);
}

int EventLoop::ResetTimerTask(timer_pointer timer) {
    return timer_mng_.ModTimer(timer);
}

int EventLoop::AddIdleTask(idle_pointer idler) {
    if (NULL == idler) {
        set_err_msg("idler pointer is null.");
        return RET::RET_FAIL;
    }
    idle_tasks_.push_back(idler);
    return RET::RET_SUCCESS;
}

int EventLoop::DelIdleTask(idle_pointer idler) {
    if (NULL == idler) {
        set_err_msg("idler pointer is null.");
        return RET::RET_FAIL;
    }
    idle_tasks_.remove(idler);
    return RET::RET_SUCCESS;
}

const std::string &EventLoop::get_err_msg() {
    return err_msg_;
}

void EventLoop::PollTask(int timeout) {
    std::vector<FiredEvent> fires;
    poll_.WaitEvent(fires, timeout);
    for (unsigned int i = 0; i < fires.size(); ++i) {
        fires_que_.push_back(fires[i]);
    }
}

void EventLoop::HandleAllIOTask() {
    while (!fires_que_.empty()) {
        FiredEvent &fire = fires_que_.front();
        iotask_pointer task = NULL;
        if (FindTask(fire.id, &task))
            task->Process(this, fire.mask);
        fires_que_.pop_front();
    }
}

bool EventLoop::FindTask(const int fd, iotask_pointer *find) {
    TaskMapItr itr = file_tasks_.find(fd);
    if (itr != file_tasks_.end()) {
        if (find)
            *find = itr->second;
        return true;
    }
    return false;
}

void EventLoop::set_err_msg(std::string msg) {
    err_msg_ = msg;
}

void EventLoop::HandleAllTimerTask() {
    std::vector<TimerImpl *> fire_timers;
    timer_mng_.GetFiredTimers(fire_timers);

    for (int i = 0; i < (int) fire_timers.size(); ++i) {
        TimerImpl &timer = *fire_timers[i];
        if (timer.get_is_loop()) {
            struct timeval now;
            gettimeofday(&now, NULL);
            timer.set_begin(now);
            AddTimerTask(&timer);
        }
        timer.Process(this, 0);
    }
}

void EventLoop::HandleAllIdleTask() {
    std::vector<idle_pointer> vecs;
    for (IdleList::iterator itr = idle_tasks_.begin(); itr != idle_tasks_.end(); ++itr) {
        if (*itr)
            vecs.push_back(*itr);
    }
    for (unsigned int i = 0; i < vecs.size(); ++i) {
        vecs[i]->Process(this, 0);
    }
}
