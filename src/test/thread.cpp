#include <iostream>
#include <unistd.h>
#include <functional>

#include "nf_thread.h"
#include "nf_mutex.h"
#include "nf_lock_queue.h"

using namespace std;
using namespace std::placeholders;

using namespace nf;

int d = 2;
LockQueue<int> que;

class Run{
public:
    void run(void *data) {
        while (true) {
            if (que.Empty())
                continue;
            cout << (char *)data << endl;
            int n;
            if (que.Next(n))
                cout << n << endl;
        }
    }

public:
    int i_ = 0;
    Mutex mutex_;
};

class WThread : public Thread {
public:
    WThread() : n_(0) {  }

    ~WThread() {  }

    void Handle(void *args) override {
        while (true) {
            que.Push(n_);
            cout << "handle " << n_++ << endl;
            sleep(1);
        }
    }

private:
    int n_;
};

int main() {
    Run run;
    Thread thread(std::bind(&Run::run, &run, _1));
    Thread thread2(std::bind(&Run::run, &run, _1));
    Thread thread3(std::bind(&Run::run, &run, _1));

    thread.Run((void *)"thread1");

    thread2.Run((void *)"thread2");

    thread3.Run((void *)"thread3");

    WThread wthread;

    wthread.Run();

    wthread.Join();

    thread.Join();
    thread2.Join();
    thread3.Join();

    cout << run.i_ << endl;

    return 0;
}
