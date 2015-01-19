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
        long usecTimeout;

    public:
        DatagramSocket(unsigned short port = 0);
        ~DatagramSocket();

        static DatagramSocket &New(unsigned short port = 0);

        bool IsValid() const;
        bool IsBound() const;

        bool SetNonBlocking();
        void SetTimeout(long);

        #if PLATFORM == PLATFORM_WINDOWS
        SOCKET GetSocket() const;
        #elif PLATFORM == PLATFORM_UNIX
        int GetSocket() const;
        #endif

        const Address &GetAddress();

        bool Send(Packet *, int flags = 0);
        int Receive(Packet *, int flags = 0);

        bool Send(Packet &, int flags = 0);
        int Receive(Packet &, int flags = 0);

        bool Send(const Address *, const Packet *, int flags = 0);
        int Receive(Address *, Packet *, int flags = 0);

        bool Send(const Address &, const Packet &, int flags = 0);
        int Receive(Address &, Packet &, int flags = 0);
    };
}

#endif