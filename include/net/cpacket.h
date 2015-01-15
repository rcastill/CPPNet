#ifndef NETCPACKET_H
#define NETCPACKET_H

#include <iostream>
#include <sstream>
#include "spacket.h"
#include "address.h"
#include "constants.h"
#include <vector>

namespace net {
    class ClientsPacket : public ServerPacket {
    private:
        Address *addresses;
        int ccount;
        static int counter;

    public:
        ClientsPacket(ServerPacket &);
        ClientsPacket(const ServerPacket &, const vector<Address> &);
        ClientsPacket(); // Initialize as request
        ~ClientsPacket();

        void Process();

        int ClientCount() const;
        const Address &operator[] (const int);

        string ToString() const;
    };
}

#endif