#include <iostream>
#include <unistd.h>
#include "src/nf_event.h"
#include <boost/asio.hpp>
#include <functional>

void cbfunc(EventService &es, Timer &task, int mask) {
    std::cout << "Timer no class: " << mask << std::endl;
}

class Cb {
public:
    void cbfunc(EventService &es, Timer &task, int mask) {
        std::cout << "Timer: " << mask << i++ << std::endl;
    }

    int i = 0;
};

int main() {
    Cb cb;
    EventService es;
    Timer timer1(1000, 1);
    timer1.Bind(&Cb::cbfunc, cb);
    timer1.Start(es);

    Timer timer2(100, 1);
    timer2.Bind(cbfunc);
    timer2.Start(es);

    es.Run();
    return 0;
}
