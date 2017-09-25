//
// @Create by Caizhili on 2017/09/25
// @Brief
//

#include "timer_task_mgr.h"

using namespace std;

TimerTaskMgr::TimerTaskMgr() {

}

TimerTaskMgr::~TimerTaskMgr() {

}

TimerTask *TimerTaskMgr::AddTimerTask(EventService &esp, unsigned long id, int timeout) {
    LogInfo("AddTimerTask for id|%lu", id);

    TimerTask *timer_ptr = new TimerTask(esp, timeout, 0);
    if (NULL == timer_ptr
            || !timer_map_.insert(make_pair(id, timer_ptr)).second) {
        if (timer_ptr)
            delete timer_ptr;
        return NULL;
    }

    return timer_ptr;
}

TimerTask *TimerTaskMgr::FindTimerTask(unsigned long id) {
    LogInfo("FindTimerTask.");

    TimerMapItr itr = timer_map_.find(id);
    if (itr == timer_map_.end())
        return NULL;

    return itr->second;
}

void TimerTaskMgr::DelTimerTask(unsigned long id) {
    LogInfo("DelTimerTask for id|%lu", id);

    TimerTask *timer = FindTimerTask(id);
    if (timer == NULL) {
        LogWarn("Not find timer task for id|%lu", id);
        return;
    }

    timer->Stop();
    timer_map_.erase(id);
    delete timer;
}
