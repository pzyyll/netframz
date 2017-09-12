#include <iostream>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <signal.h>

#include "../endpoint.h"

using namespace std;

int main(int argc, char *argv[]) {
    Endpoint ep(12233, "fe80::a00:27ff:fefa:ec5f", 1);
    string ipstr;
    cout << ep.GetPort() << endl;
    ep.GetHost(ipstr);
    cout << ipstr << endl;
    ep.GetIPStr(ipstr);
    cout << ipstr << endl;

    struct sockaddr_in6 sa_in6;
    sa_in6.sin6_family = AF_INET6;
    sa_in6.sin6_port = htons(23399);
    inet_pton(AF_INET6, "fe80::a02:27ff:fefa:ec5f", &sa_in6.sin6_addr);

    Endpoint ep2;
    ep2.SetSockAddr((struct sockaddr *)&sa_in6, sizeof(sa_in6));
    cout << ep2.GetPort() << endl;
    ep2.GetHost(ipstr);
    cout << ipstr << endl;

    return 0;
}
