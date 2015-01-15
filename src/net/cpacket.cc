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

        PutInt(sp.GetProto());
        PutInt(sp.GetId());

        PutInt(addrs.size());

        cout << "START" << endl;
        for (int i = 0; i < addrs.size(); i++) {
            PutInt(addrs[i].GetAddress());
            PutShort(addrs[i].GetPort());
        }
        cout << "END" << endl;

        ByteBuffer bb(buffer, cap);
        bb.DisableDestructor();

        int p = bb.GetInt();
        int i = bb.GetInt();
        int s = bb.GetInt();

        cout << "Protocol: " << p << endl;
        cout << "PacketID: " << i << endl;
        cout << "Connected Count: " << s << endl;

        unsigned int a;
        unsigned short b;

        for (int j = 0; j < s; j++) {
            a = (unsigned int) bb.GetInt();
            b = (unsigned short) bb.GetShort();
            cout << "(" << j + 1 << ") " << a << ":" << b << endl;
        }

    }

    /**
    * Initialize as request
    */
    ClientsPacket::ClientsPacket() : ServerPacket(0) {
        addresses = NULL;

        proto = GET_CONNECTED_CLIENTS;
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

