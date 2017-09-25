//
// @Create by Caizhili on 2017/09/25
// @Brief
//
#ifndef NF_SVR_TIMER_TASK_MGR_H
#define NF_SVR_TIMER_TASK_MGR_H

#include <unordered_map>

#include "nf_event.h"
#include "nf_event_timer_task.h"
#include "log_def.h"

class TimerTaskMgr {
public:
    typedef std::unordered_map<unsigned long, TimerTask *> TimerMap;
    typedef TimerMap::iterator TimerMapItr;

    TimerTaskMgr();

    ~TimerTaskMgr();

    TimerTask *AddTimerTask(EventService &esp, unsigned long id, int timeout);

    TimerTask *FindTimerTask(unsigned long id);

    void DelTimerTask(unsigned long id);

private:
    TimerMap timer_map_;
};

#endif //NF_SVR_TIMER_TASK_MGR_H
