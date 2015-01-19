#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <thread>
#include "dev/address.h"
#include "dev/dgsocket.h"
#include "dev/core.h"
#include "dev/packet.h"
#include "dev/spacket.h"
#include "dev/cpacket.h"
#include "dev/connpacket.h"
#include "dev/constants.h"
#include "dev/bqueue.h"

using namespace std;

namespace net {
    class Server : public DatagramSocket {
    private:
        vector<Address> clients;
        bool running;

    public:
        Server(unsigned short port = 0);
        ~Server();

        void EnableThreadingModel();

        vector<Address>&GetClients();

        bool IsRunning() const;
        void Halt();
        void Mainloop();
    };
}

#endif