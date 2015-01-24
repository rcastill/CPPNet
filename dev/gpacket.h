#ifndef NETGAMEPACKET_H
#define NETGAMEPACKET_H

#include <cstdint>
#include "spacket.h"
#include "bitfield4bytes.h"

namespace net {
    class GamePacket : public ServerPacket {
    private:
        uint32_t ack;
        Bitfield4Bytes ackb;

    public:
        GamePacket(ServerPacket &sp);
        uint32_t GetACK();
        Bitfield4Bytes &GetACKBitfield();
    };
}

#endif