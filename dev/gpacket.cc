#include "gpacket.h"

namespace net {
    GamePacket::GamePacket(ServerPacket &sp) {
        proto = sp.GetProto();
        id = sp.GetId();
        sp.Transfer(this);
        ack = (uint32_t) GetInt();
        uint32_t container = (uint32_t) GetInt();
        ackb.SetContainer(container);
    }

    uint32_t GamePacket::GetACK() {
        return ack;
    }

    Bitfield4Bytes& GamePacket::GetACKBitfield() {
        return ackb;
    }
}