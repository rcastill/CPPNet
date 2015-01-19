#ifndef NETCONSTANTS_H
#define NETCONSTANTS_H

namespace net {
    const int SERVREQ_GET_CONNECTED_CLIENTS = 0x451e6333;   // Connected clients list request
    const int SERVNOTF_CLIENT_CONNECTED = 0x29a15f16;       // Connected client notification
    const int SERVNOTF_CLIENT_DISCONNECTED = 0x3151f646;    // Disconnected client notification
    const int SERVNOTF_ACKNOWLEDGEMENT = 0x72057933;        // Notification ack for server
}

#endif