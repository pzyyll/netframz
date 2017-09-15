#include <iostream>
#include <functional>
#include <deque>
#include <unistd.h>
#include <sys/prctl.h>

#include "nf_thread.h"
#include "nf_mutex.h"
#include "nf_condition.h"

using namespace std;
using namespace std::placeholders;
using namespace nf;

deque<int> int_que;
int sum = 0;

Mutex mutex;
Condition cond(mutex);

#define THREAD_NUM 10

struct Arg {
    char name[256];
    int idx;
};

struct Arg args[THREAD_NUM];
Thread threads[THREAD_NUM];

void run(void *data) {
    struct Arg *arg = (struct Arg *)data;

    prctl(PR_SET_NAME, arg->name);

    while (true) {
        LockGuard lock(mutex);
        while (int_que.size() == 0)
            cond.Wait();
        if (int_que.size() != 0) {
            int i = int_que.back();
            int_que.pop_back();

            cout << arg->name << " ";
            cout << "i:" << i << endl;
            sum += 1;
        }
        lock.Unlock();
        sleep(1);
    }
}

int main() {
    for (int i = 0; i < THREAD_NUM; ++i) {
        snprintf(args[i].name, sizeof(args[i].name), "thread%d", i);
        args[i].idx = i;
        threads[i].SetHandle(run);
        threads[i].Run(args + i);
    }

    int n = 0;
    while (cin >> n) {
        for (int i = 0; i < n; ++i) {
            int_que.push_front(i);
        }
        //cond.Signal();
        cond.Broadcast();
    }

    return 0;
}
