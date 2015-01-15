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

        ServerPacket(int);
        ServerPacket();
        void Process();

        int GetProto() const;
        int GetId() const;
    };
}

#endif