#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <iostream>
#include "bytebuffer.h"
#include "address.h"

namespace net {

    class Packet : public ByteBuffer {
    private:
        Address address;

    public:
        Packet(char *, int);
        Packet(int);
        Packet(std::string);
        Packet();

        Address &GetAddress();

        virtual void Process() {};
    };
}

#endif