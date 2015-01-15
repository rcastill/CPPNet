#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <iostream>
#include "bytebuffer.h"

namespace net {

    class Packet : public ByteBuffer {
    public:
        Packet(char *, int);
        Packet(int);
        Packet(std::string);
        Packet();

        virtual void Process() = 0;
    };
}

#endif