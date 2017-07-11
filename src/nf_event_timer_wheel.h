//
// @Created by CaiZhili on 2017/7/6.
// @bref Timer implement by timing wheel
//       定时器精度定为 100 ms
//       但是实际情况根据内核精度确定，100 ms 的话基本都可以确定
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_WHEEL_H
#define NETFRAMZ_NF_EVENT_TIMER_WHEEL_H

#include <list>
#include <vector>

#include <sys/time.h>
#include "nf_event_timer.h"

#define PRECISION_USEC (1e5)

static inline unsigned long GetJiffersFromTime(const struct timeval &t)
{
  return (t.tv_sec * (unsigned long)(1e6 / PRECISION_USEC)
          + t.tv_usec / (unsigned long)PRECISION_USEC);
}

static inline unsigned long GetDeafaultJiffers() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return GetJiffersFromTime(now);
}

struct TimerNode {
  unsigned long expires;
  Timer timer;
};

typedef std::list<TimerNode> timer_list_t;

typedef struct NFTvecRoot {
  timer_list_t vec[NFTVR_SIZE];
} nftv_root_t;

typedef struct NFTvec {
  timer_list_t vec[NFTVN_SIZE];
} nftv_t;

class TimerWheel {
 public:
  typedef Timer timer_type;
  typedef std::vector<timer_type> timer_vec_type;
  typedef TimerNode tnode_type;

 public:

  void Init();

  int AddTimer(const struct timeval &expire, const timer_type &timer_data);

  //放到主循环去获取，粒度依据循环的精度，最小是 PRECISION_USEC
  int GetTimer(timer_vec_type &all_timer);

 private:
  void _InnerAddTimer(const tnode_type &tnode);
  timer_list_t* _GetTimerListVec(const unsigned long expires);

  int _TimerRun(timer_vec_type &all_timer, unsigned long jiffers);

 private:
  struct TvBase {
    //unsigned long size_;
    //节拍数，这里一节拍定为 100 ms;
    unsigned long clk;
    nftv_root_t tv1;
    nftv_t tv2;
    nftv_t tv3;
    nftv_t tv4;
    nftv_t tv5;
  } base_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_WHEEL_H

