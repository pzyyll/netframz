//
// @Create by Caizhili on 20170823
// @Brief nf_thread.h
//

#ifndef NF_SRC_THREAD_H
#define NF_SRC_THREAD_H

#include <functional>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>

namespace nf {

typedef std::function<void()> ThreadHandle;

class Thread {
    const static unsigned int kMaxErrMsgLen = 256;
public:
    Thread();

    explicit Thread(const ThreadHandle &handle);

    ~Thread();

    int Run();

    int Join();

    const char *GetErrMsg();

private:
    static void *ThreadRun(void *args);

private:
    ThreadHandle handle_;

    pthread_t tid_;

    char err_msg_[kMaxErrMsgLen];
};

}

#endif //NF_SRC_THREAD_H
