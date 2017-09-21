#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cassert>

#include "proto.h"
#include "proto/zonesvr.pb.h"
#include "client_worker.h"

using namespace std;
using namespace nf;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    int num;
    string name/* = "foo"*/;
    cout << "prefix name: ";
    cin >> name;
    cout << "thread num: ";
    cin >> num;

    vector<ClientWorker *> vec_workers;
    for (int i = 1; i <= num; ++i) {
        srand(time(NULL) + i);
        ostringstream iname;
        iname << name << i;
        ClientWorker *worker = new ClientWorker;
        vec_workers.push_back(worker);

        assert(worker != NULL);
        worker->Start(Endpoint(atoi(argv[2]), argv[1]), iname.str());
    }

    for (auto x: vec_workers) {
        x->Wait();
    }

    return 0;
}
