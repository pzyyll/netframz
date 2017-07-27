//
// Created by CaiZhili on 2017/6/29.
//

#include <type_traits>

#include "nf_event.h"
#include "nf_event_timer_impl.h"
#include "nf_event_iotask_impl.h"

EventLoop::EventLoop() : stop_(false) {
    poll_.Init();
}

EventLoop::~EventLoop() {

}

void EventLoop::Run() {
    do {

        if (HaveTimerTask()) HandleAllTimerTask();

        if (HaveIOEvent()) HandleIOEvent();

    } while (!stop_ && (HaveIOEvent() || HaveTimerTask()));
}

void EventLoop::Stop() {
    stop_ = true;
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

void EventLoop::HandleIOEvent() {
    int nds = poll_.WaitEvent(fires);

    //todo asyn or limit
    while (!fires.empty() && nds > 0) {
        FiredEvent &fire = fires.back();
        iotask_pointer task = NULL;
        if (FindTask(fire.id, &task)) {
            //Callback
            task->Process(this, fire.mask);
        }
        fires.pop_back();
    }
}

bool EventLoop::HaveIOEvent() {
    return (!file_tasks_.empty() ? true : false);
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

const std::string &EventLoop::get_err_msg() {
    return err_msg_;
}

void EventLoop::set_err_msg(std::string msg) {
    err_msg_ = msg;
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

bool EventLoop::HaveTimerTask() {
    return (timer_mng_.GetTimerSize() > 0);
}
