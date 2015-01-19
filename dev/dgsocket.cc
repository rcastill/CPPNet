#include "dgsocket.h"

namespace net {
    DatagramSocket::DatagramSocket(unsigned short port) {
        fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        addr = NULL;
        usecTimeout = 0;

        if (fd <= 0) return;

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(fd, (const sockaddr *) &address, sizeof(sockaddr_in)) < 0)
            return;

        else {
            socklen_t len = sizeof(address);
            getsockname(fd, (sockaddr *) &address, &len);
            addr = new Address(address);
        }
    }

    DatagramSocket::~DatagramSocket() {
        #if PLATFORM == PLATFORM_WINDOWS
        closesocket(fd);
        #elif PLATFORM == PLATFORM_UNIX
        close(fd);
        #endif
        delete addr;
    }

    bool DatagramSocket::IsValid() const {
        return fd > 0;
    }

    bool DatagramSocket::IsBound() const {
        return addr != NULL;
    }

    bool DatagramSocket::SetNonBlocking() {
        #if PLATFORM == PLATFORM_WINDOWS
        DWORD nonBlocking = 1;

        if (ioctlsocket(fd, (long) FIONBIO, &nonBlocking) != 0)
            return false;

        #elif PLATFORM == PLATFORM_UNIX
        int nonBlocking = 1;

        if (fcntl(fd, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
            return false;
        #endif

        return true;
    }

    void DatagramSocket::SetTimeout(long ms) {
        usecTimeout = ms * 1000;
    }

    #if PLATFORM == PLATFORM_WINDOWS
    SOCKET DatagramSocket::GetSocket() const {
        return fd;
    }
    #elif PLATFORM == PLATFORM_UNIX
    int DatagramSocket::GetSocket() const {
        return fd;
    }
    #endif

    const Address &DatagramSocket::GetAddress() {
        return *addr;
    }

    bool DatagramSocket::Send(Packet *packet, int flags) {
        sockaddr_in address;
        packet->GetAddress().Fill(address);
        char *data = (char *) packet->GetBuffer();
        int size = packet->Size();
        int sentBytes = sendto(fd, data, size, flags, (sockaddr *) &address, sizeof(sockaddr_in));
        return sentBytes == packet->Size();
    }

    int DatagramSocket::Receive(Packet *packet, int flags) {
        sockaddr_in from;
        socklen_t fromLength = sizeof(from);
        char *data = (char *) packet->GetBuffer();
        int size = packet->Size();

        if (usecTimeout != 0) {
            fd_set rfds;
            timeval tv;
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            tv.tv_sec = 0;
            tv.tv_usec = usecTimeout;

            if (select(fd + 1, &rfds, NULL, NULL, &tv) <= 0)
                return -1;
        }

        int recvBytes = recvfrom(fd, data, size, flags, (sockaddr *) &from, &fromLength);

        if (recvBytes <= 0)
            return 0;

        packet->GetAddress().Set(from);

        return recvBytes;
    }

    bool DatagramSocket::Send(Packet &packet, int flags) {
        return Send(&packet, flags);
    }

    int DatagramSocket::Receive(Packet &packet, int flags) {
        return Receive(&packet, flags);
    }

    bool DatagramSocket::Send(Address const *addr, Packet const *packet, int flags) {
        sockaddr_in address;
        addr->Fill(address);
        char *data = (char *) packet->GetBuffer();
        int size = packet->Size();
        int sentBytes = sendto(fd, data, size, flags, (sockaddr *) &address, sizeof(sockaddr_in));
        return sentBytes == packet->Size();
    }

    bool DatagramSocket::Send(Address const &addr, const Packet &packet, int flags) {
        return Send(&addr, &packet, flags);
    }

    int DatagramSocket::Receive(Address *sender, Packet *packet, int flags) {
        sockaddr_in from;
        socklen_t fromLength = sizeof(from);
        //char *data = new char[BUFFER_SIZE];
        char *data = (char *) packet->GetBuffer();
        int size = packet->Size();

        if (usecTimeout != 0) {
            fd_set rfds;
            timeval tv;
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            tv.tv_sec = 0;
            tv.tv_usec = usecTimeout;

            if (select(fd + 1, &rfds, NULL, NULL, &tv) <= 0)
                return -1;
        }

        int recvBytes = recvfrom(fd, data, size, flags, (sockaddr *) &from, &fromLength);

        if (recvBytes <= 0)
            return 0;

        sender->Set(from);
        //packet.SetData(data, BUFFER_SIZE);

        return recvBytes;
    }

    int DatagramSocket::Receive(Address &sender, Packet &packet, int flags) {
        return Receive(&sender, &packet, flags);
    }
}