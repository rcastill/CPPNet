#ifndef NETADDRESS_H
#define NETADDRESS_H

#include <iostream>
#include <sstream>
#include "core.h"

using namespace std;

class Address {
private:
    unsigned int address;
    unsigned short port;

public:
    static int BYTES;

    Address(unsigned char, unsigned char, unsigned char, unsigned char, unsigned short);
    Address(unsigned int, unsigned short);
    Address(const sockaddr_in &);
    Address();

    void Set(unsigned int, unsigned short);
    void Set(const sockaddr_in &);

    void Fill(sockaddr_in &) const;

    unsigned char GetA() const;
    unsigned char GetB() const;
    unsigned char GetC() const;
    unsigned char GetD() const;

    unsigned int GetAddress() const;
    unsigned short GetPort() const;

    bool operator==(const Address &);

    string ToString() const;
};

#endif /* NETADDRESS_H */