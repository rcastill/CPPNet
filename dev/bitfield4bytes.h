#ifndef BITFIELD4BYTES_H
#define BITFIELD4BYTES_H

#include <cstdint>
#include <sstream>

using namespace std;

class Bitfield4Bytes {
private:
    uint32_t container;
    bool data[32], shouldUpdate;
    void UpdateContainer();

public:
    Bitfield4Bytes();
    Bitfield4Bytes(uint32_t container);
    void SetContainer(uint32_t container);
    uint32_t GetContainer();
    bool &operator[](const size_t i);
    string String();
};

#endif