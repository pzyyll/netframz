//
// @Created by CaiZhili on 2017/6/30.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TASK_H
#define NETFRAMZ_NF_EVENT_TASK_H

#include "nf_event_config.h"

struct FiredTask {
  int id;
  int mask;
};

// 回调接口，用于触发事件回调，需用户实现
class AbstractFileCb {
 public:
  //virtual void
};

#endif //NETFRAMZ_NF_EVENT_TASK_H
