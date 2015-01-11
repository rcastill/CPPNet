#include "../../include/net/packet.h"

namespace net {

    Packet::Packet(char *data, int size, bool clean) {
        SetData(data, size, clean);
    }

    Packet::Packet(std::string stream) : Packet((char *) stream.c_str(), stream.size() + 1, false) {}

    Packet::Packet() {
        data = NULL;
        size = 0;
    }

    Packet::~Packet() {
        if (data != NULL && shouldClean)
            delete[] data;
    }

    void Packet::SetData(char *data, int size, bool clean) {
        this->data = data;
        this->size = size;
        shouldClean = clean;
    }

    char *Packet::GetData() const {
        return data;
    }

    int Packet::Size() const {
        return size;
    }
}