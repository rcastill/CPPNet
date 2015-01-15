#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <iostream>
#include <string>
#include "include/net/address.h"
#include "include/net/dgsocket.h"
#include "include/net/core.h"
#include "include/net/packet.h"
#include "include/net/spacket.h"
#include "include/net/cpacket.h"
#include "include/net/constants.h"

using namespace std;

namespace net {

    class Server : public DatagramSocket {
    private:
        vector<Address> clients;
        bool running;

    public:
        Server(unsigned short port = 0);

        bool IsRunning() const;
        void Halt();
        void Mainloop();
    };
}

#endif