//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_MNG_H
#define NETFRAMZ_NF_EVENT_TIMER_MNG_H

#include <vector>
#include <unordered_map>
#include <string>

#ifdef NF_TIMER_WHEEL

#include "nf_event_timer_wheel.h"

typedef TimerWheel<unsigned long> timers_type;
#else
#include "nf_event_timer_heap.h"
typedef TimerHeap<unsigned long> timers_type;
#endif //NF_TIMER_WHEEL

class TimerImpl;

class TimerMng {
public:
    typedef TimerImpl timer_type;
    typedef std::vector<timer_type *> fires_vec_type;
    typedef std::unordered_map<unsigned long, timer_type *> timer_map_t;
    typedef std::vector<unsigned long> vec_type;
    typedef vec_type::iterator vec_iterator;

    enum RET_STAT {
        FAIL = -1,
        SUCCESS = 0,
    };

public:
    TimerMng();

    int AddTimer(timer_type *timer);

    int ModTimer(timer_type *timer);

    int DelTimer(const unsigned long id);

    void GetFiredTimers(fires_vec_type &fired_timers);

    unsigned long GetTimerSize();

    std::string GetErrMsg();

private:
    bool _FindTimer(const unsigned long id, timer_type **timer);

    void _InnerAddTimer(timer_type *timer);

    int _DelTimer(const unsigned long id);

    unsigned long _GetDistId();

private:
    unsigned long dist_id_;
    char err_[256];

    timer_map_t timer_map_;
    timers_type timers_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_MNG_H
