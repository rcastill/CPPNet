#include "connpacket.h"

namespace net {
    ConnectionPacket::ConnectionPacket(Address addr, bool connection) {
        if (connection)
            proto = SERVNOTF_CLIENT_CONNECTED;

        else
            proto = SERVNOTF_CLIENT_DISCONNECTED;

        id = ServerPacket::count++;

        this->addr = addr;

        PutInt(proto);
        PutInt(id);
        PutInt(addr.GetAddress());
        PutShort(addr.GetPort());
    }

    ConnectionPacket::ConnectionPacket(ServerPacket &sp) {
        sp.Transfer(this);

        proto = sp.GetProto();
        id = sp.GetId();

        connection = proto == SERVNOTF_CLIENT_CONNECTED;

        unsigned int iaddr = (unsigned int) GetInt();
        unsigned short port = (unsigned short) GetShort();

        addr.Set(iaddr, port);
    }

    bool ConnectionPacket::IsConnection() const {
        return connection;
    }

    const Address &ConnectionPacket::GetNotfAddress() {
        return addr;
    }

    string ConnectionPacket::ToString() {
        stringstream ss;

        if (connection)
            ss << "Connection";

        else
            ss << "Disconnection";

        ss << " notification (" << addr.ToString() << ").";
        return ss.str();
    }
}