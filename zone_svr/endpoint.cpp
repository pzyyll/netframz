//
// @Create by Caizhili on 20170907
// @Brief wrap ip addr info
//

#include "endpoint.h"

//TODO(@Caizhili)

int SAv4From(int port, const char *addr, struct sockaddr_in *sa) {
    if (!sa)
        return -1;
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);
    //TODO
}
void SAv4To()

Endpoint::Endpoint(int port, const std::string &addr = "", bool ipv6 = false) {
    memset(&sa, 0, sizeof(sa));
    if (ipv6) {
        //TODO set ip v6;
    } else {
        SAv4 *sa = (SAv4 *)sa_;
        sa->sin_family = AF_INET;
        sa->sin_port = htons(port);
        //TODO
    }
}
