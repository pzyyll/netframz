//
// @Created by CaiZhili on 2017/7/6.
// @bref Timer implement by timing wheel
//       定时器精度定为 100 ms
//       但是实际情况根据内核精度确定，100 ms 的话基本都可以确定
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_WHEEL_H
#define NETFRAMZ_NF_EVENT_TIMER_WHEEL_H

#include <list>

#include "nf_event_timer.h"

#define NFTVN_BITS (6)
#define NFTVR_BITS (8)
#define NFTVN_SIZE (1 << NFTVN_BITS)
#define NFTVR_SIZE (1 << NFTVR_BITS)
#define NFTVN_MASK (NFTVN_SIZE - 1)
#define NFTVR_MASK (NFTVR_SIZE - 1)

template <typename Data>
struct ListNode {
  unsigned long expires;
  Data data;
};

//todo first

typedef std::list<ListNode<Timer>> timer_list;

struct tv_root {
  timer_list vec[NFTVR_SIZE];
};

class TimerWheel {
 public:

 private:
  
};

#endif //NETFRAMZ_NF_EVENT_TIMER_WHEEL_H

