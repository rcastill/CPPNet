#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <iostream>
#include "bytebuffer.h"
#include "address.h"

namespace net {

    class Packet : public ByteBuffer {
    private:
        Address address;    // New style packet

    public:
        // Parent-Matching ctors
        Packet(char *buffer, int cap);
        Packet(int cap);
        Packet();

        // Sets string as buffer
        Packet(std::string);

        // Gets assoc address
        Address &GetAddress();

        // Child packet processing
        virtual void Process() {};
    };
}

#endif