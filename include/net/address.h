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
    Address(unsigned char, unsigned char, unsigned char, unsigned char, unsigned short);
    Address(const sockaddr_in &);
    Address();

    void Set(const sockaddr_in &);

    static Address &New(unsigned char, unsigned char, unsigned char, unsigned char, unsigned short);
    static Address &New();

    void Fill(sockaddr_in &) const;

    unsigned int GetAddress() const;

    unsigned char GetA() const;
    unsigned char GetB() const;
    unsigned char GetC() const;
    unsigned char GetD() const;

    unsigned short GetPort() const;

    string ToString() const;
};

#endif /* NETADDRESS_H */