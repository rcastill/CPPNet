#ifndef NETDGSOCKET_H
#define NETDGSOCKET_H

#include "core.h"
#include "packet.h"
#include "address.h"

namespace net {
    class DatagramSocket {
    private:
        #if PLATFORM == PLATFORM_WINDOWS
        SOCKET fd;                          // socket
        #elif PLATFORM == PLATFORM_UNIX
        int fd;
        #endif
        Address *addr;                      // bound address
        long usecTimeout;                   // Timeout system

    public:
        // Create and bind UDP socket
        DatagramSocket(unsigned short port = 0);

        // Closes socket
        ~DatagramSocket();

        // Checks if socket was succesfully created
        bool IsValid() const;

        // Checks if socket was succesfully bound
        bool IsBound() const;

        // Sets non-bocking behavior
        bool SetNonBlocking();

        // Sets tuimeout (miliseconds)
        void SetTimeout(long ms);

        // Socket getter
        #if PLATFORM == PLATFORM_WINDOWS
        SOCKET GetSocket() const;
        #elif PLATFORM == PLATFORM_UNIX
        int GetSocket() const;
        #endif

        // Gets bound address
        const Address &GetAddress();

        /* Sends new-style packet (contains address), flags can be
           set for low-level networking */
        bool Send(Packet *, int flags = 0);
        int Receive(Packet *, int flags = 0);
        bool Send(Packet &, int flags = 0);
        int Receive(Packet &, int flags = 0);

        /* Send old-style packet (address must be specified), flags
           can be set for low-level networking */
        bool Send(const Address *, const Packet *, int flags = 0);
        int Receive(Address *, Packet *, int flags = 0);
        bool Send(const Address &, const Packet &, int flags = 0);
        int Receive(Address &, Packet &, int flags = 0);
    };
}

#endif