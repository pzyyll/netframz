//
// @Created by CaiZhili on 2017/7/6.
// @bref Timer implement by timing wheel linux kernel 2.6 last
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

typedef Timer data_type;

typedef struct TimerNode {
  unsigned long expires;
  data_type timer_data;
} timer_node_t;

typedef std::list<timer_node_t> timer_list_t;

typedef struct NFTvecRoot {
  timer_list_t vec[NFTVR_SIZE];
} nftv_root_t;

typedef struct NFTvec {
  timer_list_t vec[NFTVN_SIZE];
} nftv_t;

class TimerWheel {
 public:
  typedef data_type tdata_type;
  typedef std::vector<tdata_type> timer_vec_type;

 public:
  void Init();

  int AddTimer(const struct timeval &expire, const tdata_type &timer_data);

  //放到主循环去获取，粒度依据循环的精度，最小是 PRECISION_USEC
  int GetTimer(timer_vec_type &all_timer);

  unsigned long GetTimerSize();

 private:
  void _InnerAddTimer(const timer_node_t &tnode);
  timer_list_t* _GetTimerListVec(const unsigned long expires);

  int _TimerRun(timer_vec_type &all_timer, unsigned long jiffers);
  int _Cascade(nftv_t *tv, int index);

  static inline unsigned long _IndexOfTvn(unsigned long clk, unsigned int n);
  static inline unsigned long _GetJiffersFromTime(const struct timeval &t);
  static inline unsigned long _GetDeafaultJiffers();

 private:
  struct TvBase {
    unsigned long size;
    //节拍数，这里一节拍定为 100 ms;
    unsigned long clk;
    nftv_root_t tv;
    nftv_t tv1;
    nftv_t tv2;
    nftv_t tv3;
    nftv_t tv4;
  } base_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_WHEEL_H

