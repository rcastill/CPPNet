#ifndef NETADDRESS_H
#define NETADDRESS_H

#include <iostream> // std::string
#include <sstream>  // std::stringstream
#include "core.h"   // sockaddr_in

using namespace std;

class Address {
private:
    unsigned int address;   // Integer representation of IPV4
    unsigned short port;    // Port

public:
    static int BYTES;       // It's weight when serialized (sizeof(unsigned int) + sizeof(unsigned short) ~ 6bytes)

    //IPV4  + port ctor (e.g. Address(127, 0, 0, 1, 8080))
    Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);

    // IPV4 repr + port ctor
    Address(unsigned int address, unsigned short port);

    // sockaddr_in ctor, uses addr.sin_addr.s_addr and addr.sin_port
    Address(const sockaddr_in &addr);

    // Default constructor, initializes fields to 0
    Address();

    /* Corresponding ctor implementation. They are used by the constructors and made accesible
       for filling when created through default ctor. */
    void Set(unsigned char, unsigned char, unsigned char, unsigned char, unsigned short);
    void Set(unsigned int, unsigned short);
    void Set(const sockaddr_in &);
    void Set(const Address& addr);

    // Reset fields to 0
    void Clear();

    // Fills a sockaddr_in with address/port
    void Fill(sockaddr_in &) const;

    /* Get different bytes from IPV4: A.B.C.D:PORT */
    unsigned char GetA() const;
    unsigned char GetB() const;
    unsigned char GetC() const;
    unsigned char GetD() const;

    // Get IPV4 representation copy
    unsigned int GetAddress() const;

    // Get port copy
    unsigned short GetPort() const;

    // Compares address and port
    bool operator==(const Address &);

    // String representation of the form A.B.C.D:PORT
    string ToString() const;
};

#endif /* NETADDRESS_H */