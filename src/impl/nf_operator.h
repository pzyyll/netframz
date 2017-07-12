//
// @Created by CaiZhili on 2017/7/12.
// @bref 
//

#ifndef NETFRAMZ_NF_OPERATOR_H
#define NETFRAMZ_NF_OPERATOR_H

#include <functional>

class EventLoop;

class Operator {
  friend class EventLoop;
  typedef std::function<void (EventLoop&, Operator&, int)> cb_type;
 public:
  template<typename Func, typename Obj>
  void Bind(Func &&handle, Obj &&obj) {
    cb_ = std::bind(handle, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
  }

  void Bind(cb_type handle) {
    cb_ = handle;
  }

 protected:
  void Process(EventLoop &loop, Operator &optor, int mask) {
    if (cb_)
      cb_(loop, optor, mask);
  }

 private:
  cb_type cb_;
};

#endif //NETFRAMZ_NF_OPERATOR_H
