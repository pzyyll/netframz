#include "proto.h"

namespace proto {

int ReadMsgHeader(struct MsgHeader *msg_header, const char *buffer) {
    const struct MsgHeader *sdata = (const struct MsgHeader *)buffer;
    msg_header->len = ntohs(sdata->len);
    msg_header->type = ntohs(sdata->type);

    return sizeof(*sdata);
}

int WriteMsgHeader(const struct MsgHeader *msg_header, char *buffer) {
    struct MsgHeader *sdata = (struct MsgHeader *)buffer;
    sdata->len = htons(msg_header->len);
    sdata->type = htons(msg_header->type);

    return sizeof(*sdata);
}

}

