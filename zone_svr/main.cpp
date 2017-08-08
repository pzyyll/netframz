#include <iostream>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <signal.h>

#include "server.h"
#include "mem_check.h"

using namespace std;


BaseServer svr;
static void TermSigHandler(int sig) {
    (void)(sig);
    svr.Stop();

#ifdef MEM_CHECK
    MemCheck &mem_check = MemCheck::GetMutableInstance();
    cout << "\nnon delete mem size : " << mem_check.GetSize() << endl;

    for (struct NodeInfo *node = mem_check.GetNext(true);
         node != NULL;
         node = mem_check.GetNext()) {

        cout << node->file_name
             << ":" << node->line
             << " size " << node->size
             << endl;
    }
#endif //MEM_CHECK

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
