#include <iostream>
#include <unistd.h>
#include <functional>

#include "nf_thread.h"
#include "nf_mutex.h"

using namespace std;
using namespace nf;

class Run{
public:
    void run() {
        int n = 2000000;
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
    Thread thread(std::bind(&Run::run, &run));
    Thread thread2(std::bind(&Run::run, &run));
    Thread thread3(std::bind(&Run::run, &run));

    thread.Run();

    thread2.Run();

    thread3.Run();


    thread.Join();
    thread2.Join();
    thread3.Join();

    cout << run.i_ << endl;

    return 0;
}
