#include "../../include/net/cpacket.h"

namespace net {
    int ClientsPacket::counter = 0;

    ClientsPacket::ClientsPacket(ServerPacket &sp) {
        proto = sp.GetProto();
        id = sp.GetId();
        sp.Transfer(this);
    }

    ClientsPacket::ClientsPacket(const ServerPacket &sp, const vector<Address> &addrs) :
            ServerPacket(sizeof(int) + addrs.size() * Address::BYTES) {

        addresses = NULL;

        PutInt(sp.GetProto()); // Protocol
        PutInt(sp.GetId()); // Packet ID

        PutInt(addrs.size()); // Client count

        for (int i = 0; i < addrs.size(); i++) {
            PutInt(addrs[i].GetAddress()); // Client (i) 's address
            PutShort(addrs[i].GetPort()); // Client (i) 's port
        }
    }

    /**
    * Initialize as request
    */
    ClientsPacket::ClientsPacket() : ServerPacket(0) {
        addresses = NULL;

        proto = SERVREQ_GET_CONNECTED_CLIENTS;
        id = ClientsPacket::counter++;
        PutInt(proto);
        PutInt(id);
    }

    ClientsPacket::~ClientsPacket() {
        if (addresses != NULL)
            delete[] addresses;
    }

    void ClientsPacket::Process() {
        ccount = GetInt();

        cout << "ccount = " << ccount << endl;

        addresses = new Address[ccount];

        for (int i = 0; i < ccount; i++) {
            unsigned int address = (unsigned int) GetInt();
            unsigned short port = (unsigned short) GetShort();

            addresses[i].Set(address, port);
        }
    }

    int ClientsPacket::ClientCount() const {
        return ccount;
    }

    const Address &ClientsPacket::operator[](int const i) {
        return addresses[i];
    }

    string ClientsPacket::ToString() const {
        stringstream ss;

        for (int i = 0; i < ccount; i++) {
            ss << addresses[i].ToString();

            if (i < ccount - 1)
                ss << endl;
        }

        return ss.str();
    }
}

