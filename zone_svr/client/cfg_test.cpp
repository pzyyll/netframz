#include <iostream>
#include <string>
#include <vector>

#include "config.h"

using namespace std;

int main(int argc, char *argv[]) {
    CConfig cfg(argv[1]);

    int port;
    cfg.GetInt(port, "port", 23399);

    cout << port << endl;

    string ip;
    cfg.GetString(ip, "ip", "127.0.0.1");
    cout << ip << endl;

    unsigned long long ull;
    cfg.GetULL(ull, "ull", 3344);
    cout << ull << endl;

    vector<string> ips;
    cfg.GetStringArray(ips, "ips");
    for (auto x: ips) {
        cout << x << endl;
    }

    vector<int> ports;
    cfg.GetIntArray(ports, "ports");
    for (auto x: ports) {
        cout << x << endl;
    }

    vector<unsigned long long> ulls;
    cfg.GetULLArray(ulls, "ulls");
    for (auto x: ulls) {
        cout << x << endl;
    }

    cout << "====" << endl;
    return 0;
}
