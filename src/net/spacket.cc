#include "../../include/net/spacket.h"

namespace net {
    int ServerPacket::BYTES = sizeof(int) * 2;

    ServerPacket::ServerPacket(int cap) : Packet(BYTES + cap) {}

    ServerPacket::ServerPacket() : Packet(BUFFER_SIZE) {}

    void ServerPacket::Process() {
        proto = GetInt();
        id = GetInt();
    }

    int ServerPacket::GetProto() const {
        return proto;
    }

    int ServerPacket::GetId() const {
        return id;
    }
}