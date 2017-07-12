//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#include <cstdio>
#include "nf_event_timer_mng.h"

TimerMng::TimerMng() : dist_id_(1) {
    timers_.Init();
}

int TimerMng::AddTimer(timer_type &timer) {
  if (_FindTimer(timer.get_id(), NULL)) {
    snprintf(err_, sizeof(err_), "Timer already add to timer loop!");
    return FAIL;
  }

  ModTimer(timer);
  return SUCCESS;
}

int TimerMng::ModTimer(timer_type &timer) {
  if (_FindTimer(timer.get_id(), NULL)) {
    _DelTimer(timer.get_id());
  }

  _InnerAddTimer(timer);

  return SUCCESS;
}

int TimerMng::DelTimer(const unsigned long id) {
  return _DelTimer(id);
}

void TimerMng::GetFiredTimers(std::vector<timer_type> &fired_timers) {
  vec_type fire_ids;
  timers_.GetTimer(fire_ids);

  for (vec_iterator itr = fire_ids.begin(); itr != fire_ids.end(); ++itr) {
    timer_type find;
    if (_FindTimer(*itr, &find))
      fired_timers.push_back(find);
  }
}

unsigned long TimerMng::GetTimerSize() {
  return timers_.GetTimerSize();
}

std::string TimerMng::GetErrMsg() {
  return std::string(err_);
}

bool TimerMng::_FindTimer(const unsigned long id, TimerMng::timer_type *timer) {
  timer_map_t::iterator find = timer_map_.find(id);
  if (find == timer_map_.end()) {
    return false;
  }

  if (timer)
    *timer = find->second;

  return true;
}

void TimerMng::_InnerAddTimer(TimerMng::timer_type &timer) {
  timer.set_id(_GetDistId());
  timer_map_.insert(std::make_pair(timer.get_id(), timer));
  timers_.AddTimer(timer.GetExpireTime(), timer.get_id());
}

int TimerMng::_DelTimer(const unsigned long id) {
  if (timer_map_.erase(id))
    return SUCCESS;
  return FAIL;
}

unsigned long TimerMng::_GetDistId() {
  return dist_id_++;
}


