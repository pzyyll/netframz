#include <iostream>
#include <unistd.h>
#include <functional>

#include "nf_thread.h"
#include "nf_mutex.h"

using namespace std;
using namespace std::placeholders;

using namespace nf;
int d = 2;

class Run{
public:
    void run(void *data) {
        cout << *((int *)data) << endl;
        int n = 2000000;
        cout << (void *)&n << endl;
        while (n-- > 0) {
            MutexGuard lock(mutex_);
 //           cout << "run " << i_++ << endl;
            ++i_;
        }
    }

public:
    int i_ = 0;
    Mutex mutex_;
};

int main() {
    Run run;
    Thread thread(std::bind(&Run::run, &run, _1));
    Thread thread2(std::bind(&Run::run, &run, _1));
    Thread thread3(std::bind(&Run::run, &run, _1));

    thread.Run(&d);

    thread2.Run(&d);

    thread3.Run(&d);


    thread.Join();
    thread2.Join();
    thread3.Join();

    cout << run.i_ << endl;

    return 0;
}
