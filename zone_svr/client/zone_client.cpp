#include <iostream>
#include <map>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "proto.h"
#include "proto/zonesvr.pb.h"
#include "client_api.h"

using namespace std;

Client cli;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    strncpy(cli.conn_ip, argv[1], sizeof(cli.conn_ip));
    cli.conn_port = atoi(argv[2]);
    cli.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (cli.fd < 0) {
        cout << strerror(errno) << endl;
        return -1;
    }

    CliRun(&cli);

    return 0;
}
