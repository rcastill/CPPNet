#ifndef NETDGSOCKET_H
#define NETDGSOCKET_H

#include "core.h"
#include "packet.h"
#include "address.h"

namespace net {
    class DatagramSocket {
    private:
        #if PLATFORM == PLATFORM_WINDOWS
        SOCKET fd;
        #elif PLATFORM == PLATFORM_UNIX
        int fd;
        #endif
        Address *addr;

    public:
        DatagramSocket(unsigned short port = 0);
        ~DatagramSocket();

        static DatagramSocket &New(unsigned short port = 0);

        bool IsValid() const;
        bool IsBound() const;

        bool SetNonBlocking();

        const Address &GetAddress();

        bool Send(const Address &, const Packet &, int flags = 0);
        int Receive(Address &, Packet &, int flags = 0);
    };
}

#endif