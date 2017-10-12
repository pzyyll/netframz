//
// @Create By Caizhili on 20170906
// @Brief
//

#include "zone.h"

Zone::Zone() {

}

Zone::~Zone() {

}

int Zone::Update() {
    //TODO
    return 0;
}

void Zone::QueueSession(ZoneSession *session) {
    queue_session_.Push(session);
}
