#include <iostream>
#include <functional>
#include <unistd.h>

#include "server.h"

using namespace std;

int main(int argc, char *argv[]) {

    TServer svr;

    if (svr.Init(argc, argv) < 0) {
        cout << "init fail." << endl;
        return -1;
    }

    svr.Run();

    return 0;
}
