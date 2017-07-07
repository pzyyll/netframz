//
// @Created by CaiZhili on 2017/7/6.
// @bref Timer implement by min-heap
//       这个定时器只提供添加和获取
//       定时器精度定为 100 ms 吧，虽然可以精确到 1 us, 
//       但是实际情况根据内核精度确定，100 ms 的话基本都可以确定
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_IMPLV1_H
#define NETFRAMZ_NF_EVENT_TIMER_IMPLV1_H

#include <queue>
#include <vector>
#include <sys/time.h>
#include "nf_event_timer.h"

#define PRECISION_USEC 100000
#define GET_PRECISION_USEC(_usec) (_usec / PRECISION_USEC * PRECISION_USEC)

inline int CompareTime(const struct timeval &tv1, const struct timeval &tv2){
  if (tv1.tv_sec > tv2.tv_sec) return 1;
  if (tv2.tv_sec > tv1.tv_sec) return -1;
  return static_cast<int>(tv1.tv_usec - tv2.tv_usec);
}

class TimerHeap {
 public:
  typedef Timer TimerDataType;

  enum EIXTSTATUS {
    FAIL = -1,
    SUCCESS = 0,
  };

  struct TimerNode {
    struct timeval expire_time;
    TimerDataType timer_data;
  };

  class LtTimerNode {
   public:
    bool operator()(const TimerNode &a, const TimerNode &b) {
      return CompareTime(a.expire_time, b.expire_time) > 0;
    }
  };

  typedef std::priority_queue<TimerNode, std::vector<TimerNode>, LtTimerNode> heap_type;

 public:
  int AddTimer(struct timeval expire, const TimerDataType &timer_data) {
    expire.tv_usec = GET_PRECISION_USEC(expire.tv_usec);
    struct timeval now;
    gettimeofday(&now, NULL);
    now.tv_usec = GET_PRECISION_USEC(now.tv_usec);
    //expire < now
    if (CompareTime(expire, now) < 0) return FAIL;
    struct TimerNode timer_node = {expire, timer_data};

    timer_heap_.push(timer_node);

    return SUCCESS;
  }

  //放到主循环去获取，粒度依据循环的精度，最小是 PRECISION_USEC
  int GetTimer(std::vector<TimerDataType> &all_timer) {
    struct timeval now;
    gettimeofday(&now, NULL);
    now.tv_usec = GET_PRECISION_USEC(now.tv_usec);

    while (!timer_heap_.empty()) {
      const struct TimerNode &top = timer_heap_.top();
      if (CompareTime(top.expire_time, now) > 0)
        break;
      all_timer.push_back(top.timer_data);
      timer_heap_.pop();
    }

    return all_timer.size();
  }

 private:
  heap_type timer_heap_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_IMPLV1_H
