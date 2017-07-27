#include <iostream>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <signal.h>

#include "server.h"
#include "mem_check.h"

using namespace std;


TServer svr;
static void TermSigHandler(int sig) {
    svr.Stop();

    /*
    cout << "\nnon delete mem size : " << GetMemMap().size() << endl;

    for (auto x: GetMemMap()) {
        cout << x.second.file_name << ":"
             << x.second.line << " size:"
             << x.second.size << endl;
        //delete x.first;
    }
    cout << "end" << endl;
    GetMemMap().clear();
    */
}

int main(int argc, char *argv[]) {
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = TermSigHandler;
    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGQUIT, &sig, NULL);
    sigaction(SIGABRT, &sig, NULL);

    if (svr.Init(argc, argv) < 0) {
        cout << "init fail." << endl;
        return -1;
    }

    svr.Run();

    return 0;
}
