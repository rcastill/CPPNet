#ifndef NETCPACKET_H
#define NETCPACKET_H

#include <iostream>
#include <sstream>
#include "spacket.h"
#include "address.h"
#include "constants.h"
#include <vector>
#include "../bclient.h"

namespace net {
    class ClientsPacket : public ServerPacket {
    private:
        Address *addresses; // Addresses array
        int ccount;         // Addresses count

    public:
        // Constructs clients packet from ServerPacket buffer
        ClientsPacket(ServerPacket &sp);

        ClientsPacket(const ServerPacket &sp, void *hybaddrs, int step, size_t size);

        ClientsPacket(const ServerPacket &sp, const vector<BackendClient> &addrs);

        /* Constructs answer to SERVREQ_GET_CONNECTED_CLIENTS request.
           sp is used for match request id, and addrs is the container
           of the connected clients, the data is serialized and sent */
        ClientsPacket(const ServerPacket &sp, vector<Address> &addrs);

        // Initialize as request
        ClientsPacket();

        // delete[] addresses;
        ~ClientsPacket();

        // Fills addresses and ccount from ServerPacket buffer
        void Process();

        // Connected clients list size
        int ClientCount() const;

        // Access elements of the array
        const Address &operator[] (const int);

        // Represents address array
        string ToString() const;
    };
}

#endif