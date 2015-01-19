#ifndef NETSPACKET_H
#define NETSPACKET_H

#include "packet.h"
#include "bytebuffer.h"

namespace net {
    class ServerPacket : public Packet {
    protected:
        int proto, id;  // Packet protocol, Packet id

    public:
        static int BYTES;   // Serialization size by its own (sizeof(int) * 2)
        static int count;   // ServerPacket counter (used for id)

        // Sets data and serializes it
        ServerPacket(int proto, int id);

        // Allocates BYTES + cap bytes
        ServerPacket(int cap);

        // Parent-Matching ctor
        ServerPacket();

        // Process implemtation
        void Process();

        // Gets proto and id
        int GetProto() const;
        int GetId() const;
    };
}

#endif