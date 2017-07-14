//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_CONFIG_H
#define NETFRAMZ_NF_EVENT_CONFIG_H

#define ret_val_if_fail(pointer, ret) if (NULL == pointer) return ret;

enum RET {
  RET_SUCCESS    = 0,
  RET_FAIL       = -1,
};

enum EVSTAT {
  EV_NONE        = 0,
  EV_READABLE    = 0X001,
  EV_WRITEABLE   = 0X002,
  EV_HUP         = 0X004,
  EV_RDHUP       = 0X008,    //EPOLL since linux 2.6.17 EPOLLRDHUP
  EV_ERR         = 0X100,
};

struct task_data_t {
  union {
    void *ptr;
    int fd;
    unsigned long id;
  } data;
};

#endif //NETFRAMZ_NF_EVENT_CONFIG_H
