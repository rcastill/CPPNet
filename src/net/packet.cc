#include "../../include/net/packet.h"

namespace net {

    Packet::Packet(char *data, int size) : ByteBuffer(data, size) {}

    Packet::Packet(int cap) : ByteBuffer(cap) {}

    Packet::Packet(std::string stream) : Packet((char *) stream.c_str(), stream.size() + 1) {
        DisableDestructor();
    }

    Packet::Packet() : ByteBuffer() {}
}