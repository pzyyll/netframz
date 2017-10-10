//
// @Create by Caizhili on 2017/10/10
// @Brief
//

#ifndef NF_SVR_ZONE_CONNECTOR_H
#define NF_SVR_ZONE_CONNECTOR_H

#include "nf_event.h"
#include "connector.h"

class ZoneConnector : public Connector {
public:
    ZoneConnector(EventService &es, int fd);

    ~ZoneConnector();

protected:
    void HandleInput(ErrCode &err_code) /*override*/;

    void HandleOutput(ErrCode &err_code) /*override*/;

private:
    //TODO
};

#endif //NF_SVR_ZONE_CONNECTOR_H
