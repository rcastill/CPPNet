#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <list>
#include <map>
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
#include "bclient.h"

using namespace std;

namespace net {
    class Server : public DatagramSocket {
    private:
        vector<BackendClient> clients;  // Connected clients collection
        bool running;                   // Server state

    public:
        // Parent-Matching ctor (auto allocated port by default)
        Server(unsigned short port = 0);

        // Internal use dctor
        ~Server();

        /* Enable threading for sending pendant packets. If not
           enabled, pendant packets will be sent at the end of
           the mainloop, so if the socket is not set non-blocking,
           pendant packets will be sent just after something
           is received or timeout (if set) is completed. */
        void EnableThreadingModel();

        // Gets connected clients collection
        vector<BackendClient>&GetClients();

        // Tells server state
        bool IsRunning() const;

        // Halts server
        void Halt();

        // Server mainloop
        void Mainloop();
    };
}

#endif