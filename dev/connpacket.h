#ifndef NETCONNPACKET_H
#define NETCONNPACKET_H

#include "spacket.h"
#include "constants.h"
#include <sstream>

namespace net {
    class ConnectionPacket : public ServerPacket {
    private:
        bool connection;
        Address addr;

    public:
        ConnectionPacket(Address addr, bool connection);
        ConnectionPacket(ServerPacket &sp);

        bool IsConnection() const;
        const Address &GetNotfAddress();

        string ToString();
    };
}

#endif