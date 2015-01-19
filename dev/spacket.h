#ifndef NETSPACKET_H
#define NETSPACKET_H

#include "packet.h"
#include "bytebuffer.h"

namespace net {
    class ServerPacket : public Packet {
    protected:
        int proto, id;

    public:
        static int BYTES;
        static int count;

        ServerPacket(int, int);
        ServerPacket(int);
        ServerPacket();
        void Process();

        int GetProto() const;
        int GetId() const;
    };
}

#endif