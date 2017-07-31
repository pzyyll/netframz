#ifndef NF_ERR_CODE_H
#define NF_ERR_CODE_H

#include <string>

class ErrCode {
public:
    enum RET {
        FAIL = -1,
        SUCCESS = 0,
    };
public:
    ErrCode(int ret = 0, std::string err_msg = "") : ret_(ret), err_msg_(err_msg) {  }
    ~ErrCode() {  }

    void set_ret(int ret) { ret_ = ret; }
    int get_ret() { return ret_; }

    void set_err_msg(const std::string &err_msg) { err_msg_ = err_msg; }
    std::string get_err_msg() { return err_msg_; }

private:
    int ret_;
    std::string err_msg_;
};

#endif //NF_ERR_CODE_H
