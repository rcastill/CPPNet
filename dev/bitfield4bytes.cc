#include "bitfield4bytes.h"

Bitfield4Bytes::Bitfield4Bytes() {
    for (size_t i = 0; i < 32; i++)
        data[i] = false;

    shouldUpdate = true;
}

Bitfield4Bytes::Bitfield4Bytes(uint32_t container) {
    SetContainer(container);
}

void Bitfield4Bytes::SetContainer(uint32_t container) {
    this->container = container;

    for (int i = 31; i >= 0; i--)
        data[31 - i] = (bool) ((container >> i) & 0x1);

    shouldUpdate = false;
}

void Bitfield4Bytes::UpdateContainer() {
    container = 0;
    for (int i = 31; i >= 0; i--) {
        if (data[31 - i]) {
            container |= ((uint32_t) 1) << i;
        }
    }
}

uint32_t Bitfield4Bytes::GetContainer() {
    if (shouldUpdate) {
        UpdateContainer();
        shouldUpdate = false;
    }

    return container;
}

bool &Bitfield4Bytes::operator[](const size_t i) {
    shouldUpdate = true;
    return data[i % 32];
}

string Bitfield4Bytes::String() {
    stringstream ss;
    ss << "[" << data[0];
    for (size_t i = 1; i < 32; i++)
        ss << ", " << data[i];
    ss << "]";
    return ss.str();
}