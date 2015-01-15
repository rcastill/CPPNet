#include "../../include/net/address.h"

int Address::BYTES = sizeof(unsigned int) + sizeof(unsigned short);

Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port) {
    address = (a << 24) | (b << 16) | (c << 8) | d;
    this->port = port;
}

Address::Address(unsigned int address, unsigned short port) {
    Set(address, port);
}

Address::Address(const sockaddr_in &addr) {
    Set(addr);
}

Address::Address() {
    address = 0;
    port = 0;
}

void Address::Set(unsigned int address, unsigned short port) {
    this->address = address;
    this->port = port;
}

void Address::Set(const sockaddr_in &addr) {
    address = ntohl(addr.sin_addr.s_addr);
    port = ntohs(addr.sin_port);
}

void Address::Fill(sockaddr_in &sockaddrIn) const {
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_addr.s_addr = htonl(address);
    sockaddrIn.sin_port = htons(port);
}

unsigned int Address::GetAddress() const {
    return address;
}

unsigned char Address::GetA() const {
    return (unsigned char) (address >> 24);
}

unsigned char Address::GetB() const {
    return (unsigned char) (address >> 16);
}

unsigned char Address::GetC() const {
    return (unsigned char) (address >> 8);
}

unsigned char Address::GetD() const {
    return (unsigned char) address;
}

unsigned short Address::GetPort() const {
    return port;
}

bool Address::operator==(const Address &other) {
    return (address == other.address && port == other.port);
}

string Address::ToString() const {
    stringstream ss;
    ss << (int) GetA() << "." << (int) GetB() << "." << (int) GetC() << "." << (int) GetD() << ":" << port;
    return ss.str();
}