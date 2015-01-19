#include "spacket.h"

namespace net {
    int ServerPacket::BYTES = sizeof(int) * 2;
    int ServerPacket::count = 0;

    ServerPacket::ServerPacket(int proto, int id) : Packet(BYTES) {
        this->proto = proto;
        this->id = id;

        PutInt(proto);
        PutInt(id);
    }

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