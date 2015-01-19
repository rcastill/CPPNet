#ifndef NETCONNPACKET_H
#define NETCONNPACKET_H

#include "spacket.h"
#include "constants.h"
#include <sstream>

namespace net {
    class ConnectionPacket : public ServerPacket {
    private:
        bool connection;    // Tells wether represents a connection or disconnection
        Address addr;       // Address in that connected/disconnected

    public:
        // fils addr and connection
        ConnectionPacket(Address addr, bool connection);

        // constructs from server packet buffer
        ConnectionPacket(ServerPacket &sp);

        // returns connection
        bool IsConnection() const;

        // Gets addr (Parent class Packet already has GetAddress)
        const Address &GetNotfAddress();

        // Connection/Disconnection notification: addr.ToString()
        string ToString();
    };
}

#endif