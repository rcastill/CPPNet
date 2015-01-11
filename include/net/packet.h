#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <iostream>

namespace net {

    class Packet {
    protected:
        int size;
        char *data;
        bool shouldClean;

    public:
        Packet(char *, int, bool clean=true);
        Packet(std::string);
        void SetData(char *, int, bool clean=true);
        Packet();
        ~Packet();

        char *GetData() const;
        int Size() const;
    };
}

#endif