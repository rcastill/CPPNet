#include "bclient.h"

#include "server.h"

BackendClient::BackendClient(Address &address, Server *server) : Address(address), brothers(server->GetClients()) {
    this->server = server;
}

BackendClient::BackendClient(Address &address, Server &server) : BackendClient(address, &server) {}

void BackendClient::Process(ServerPacket &packet) {
    packet.Process();
    int proto = packet.GetProto();
    int id = packet.GetId();

    switch (proto) {
        case SERVREQ_GET_CONNECTED_CLIENTS: {
            cout << ToString() << " requests clients list." << endl;
            ClientsPacket clientsPacket(packet, brothers);
            clientsPacket.GetAddress().Set(*this);
            server->Send(clientsPacket);
            break;
        }

        case SERVNOTF_ACKNOWLEDGEMENT: {
            cout << "Received notification ACK (" << id << "). ";

            if (IsPendant(id))
                pendant.remove(id);

            else
                cout << "Dropped.";

            cout << endl;
            break;
        }

        default:
            cout << "Unkonwn protocol. Dropped." << endl;
            break;
    }
}

bool BackendClient::IsPendant(int id) {
    list<int>::iterator it;
    for (it = pendant.begin(); it != pendant.end(); it++)
        if (*it == id) return true;

    return false;
}

void BackendClient::SetPendant(int id) {
    pendant.push_back(id);
}
