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

#define NFTVN_BITS (6)
#define NFTVR_BITS (8)
#define NFTVN_SIZE (1 << NFTVN_BITS)
#define NFTVR_SIZE (1 << NFTVR_BITS)
#define NFTVN_MASK (NFTVN_SIZE - 1)
#define NFTVR_MASK (NFTVR_SIZE - 1)
#define PRECISION_USEC (1e5)

static inline unsigned long GetJiffersFromTime(const struct timeval &t)
{
    return (t.tv_sec * (unsigned long)(1e6 / PRECISION_USEC) + t.tv_usec / (unsigned long)PRECISION_USEC);
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

//todo first

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

 public:

  void Init();

  int AddTimer(struct timeval expire, const timer_type &timer_data);

  //放到主循环去获取，粒度依据循环的精度，最小是 PRECISION_USEC
  int GetTimer(std::vector<timer_type> &all_timer);

 private:
  struct TvBase {
    //unsigned long size_;
    //节拍数，这里一节拍定为 100 ms;
    unsigned long timer_jiffers_;
    nftv_root_t tv1_;
    nftv_t tv2_;
    nftv_t tv3_;
    nftv_t tv4_;
    nftv_t tv5_;
  } base_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_WHEEL_H

