#include <iostream>
#include <sstream>
#include <map>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "proto.h"
#include "proto/zonesvr.pb.h"
#include "robot_api.h"
#include "nf_thread.h"

using namespace std;
using namespace nf;

void worker(int argc, char *argv[], const std::string &name) {
    Client cli;
    strncpy(cli.conn_ip, argv[1], sizeof(cli.conn_ip));
    cli.conn_port = atoi(argv[2]);
    cli.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (cli.fd < 0) {
        cout << strerror(errno) << endl;
        return;
    }

    cli.name.assign(name);
    CliRun(&cli);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }
    int num;
    cin >> num;

    string name = "foo";
    for (int i = 1; i <= num; ++i) {
        srand(time(NULL) + i);
        ostringstream iname;
        iname << name << i;
        pid_t pid = fork();
        if (pid == 0) {
            worker(argc, argv, iname.str());
        }
    }

    return 0;
}
