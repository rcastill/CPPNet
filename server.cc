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
            Packet packet(BUFFER_SIZE);

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

            cout << address.ToString() << " says: " << string(packet.GetData()) << endl;

            cout << "Sending greetings!" << endl;
            string greetings = "Greetings!";

            Send(address, Packet(greetings));
            cout << "Greetings sent" << endl;
        }
    }

}

int main(int argc, char **argv) {
    unsigned short port = 50000;

    if (argc == 2)
        port = (unsigned short) atoi(argv[1]);

    if (net::NetworkInit()) {
        net::Server server(port);

        if (server.IsValid() && server.IsBound()) {
            cout << "Server bound @ " << server.GetAddress().ToString() << endl;
            server.SetTimeout(2000);
            server.Mainloop();
            net::NetworkShutdown();
        }

        cout << "Not valid or bound" << endl;
    }

    else
        cout << "Could not initialize network" << endl;

    return 0;
}