#include "server.h"


namespace net {
    Server::Server(unsigned short port) : DatagramSocket(port) {
        running = true;
    }

    bool Server::IsRunning() const {
        return running;
    }

    void Server::Halt() {
        running = false;
    }

    void Server::Mainloop() {
        while (running) {
            Address address;
            ServerPacket packet;

            cout << "Waiting for packet... ";
            int bytes = Receive(address, packet);

            if (bytes == -1) {
                cout << "Timeout!" << endl;
                continue;
            }

            cout << "Received " << bytes << " bytes." << endl;

            if (bytes == 0) continue;

            unsigned int i;
            for (i = 0; i < clients.size(); i++)
                if (clients[i] == address) break;

            if (i == clients.size()) {
                clients.push_back(address);
                cout << "Client connected (" << address.ToString() << ")" << endl;
            }

            packet.Process();
            int proto = packet.GetProto();
            int id = packet.GetId();

            switch (proto) {
                case SERVREQ_GET_CONNECTED_CLIENTS: // Clients Request
                    cout << address.ToString() << " requests clients list" << endl;
                    Send(address, ClientsPacket(packet, clients));
                    break;

                default:
                    cout << "Unkonwn protocol. Dropped" << endl;
                    break;
            }
        }
    }

}

int main(int argc, char **argv) {
    unsigned short port = 50000;
    long timeout = 0;

    if (argc == 2)
        port = (unsigned short) atoi(argv[1]);

    if (argc == 3) {
        port = (unsigned short) atoi(argv[1]);
        timeout = atol(argv[2]);
    }

    if (net::NetworkInit()) {
        net::Server server(port);

        if (server.IsValid() && server.IsBound()) {
            cout << "Server bound @ " << server.GetAddress().ToString() << endl;
            cout << "Receive timeout set to " << timeout << endl;

            // FIXME Timeout not working
            if (timeout != 0)
                server.SetNonBlocking();

            server.SetTimeout(timeout);
            server.Mainloop();
            net::NetworkShutdown();
        }

        cout << "Not valid or bound" << endl;
    }

    else
        cout << "Could not initialize network" << endl;

    return 0;
}