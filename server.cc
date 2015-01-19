#include "server.h"

#if PLATFORM == PLATFORM_WINDOWS
#include "dev/mingw.thread.h"
#elif PLATFORM == PLATFORM_UNIX
#include <thread>
#endif

namespace net {
    bool usingThreads = false;
    BlockingQueue<ServerPacket*> bsqueue;
    queue<ServerPacket*> squeue;

    list<int> pendant;
    thread sthread;

    struct CommonQueue {
    private:
        BlockingQueue<ServerPacket*> *bq;
        queue<ServerPacket*> *q;

    public:
        CommonQueue() {
            bq = NULL;
            q = NULL;
        }

        CommonQueue(BlockingQueue<ServerPacket*> *kiwi) {
            bq = kiwi;
            q = NULL;
        }

        CommonQueue(queue<ServerPacket*> *kiwi) {
            bq = NULL;
            q = kiwi;
        }

        CommonQueue &operator=(CommonQueue&& other) {
            bq = other.bq;
            q = other.q;

            other.bq = NULL;
            other.q = NULL;

            return *this;
        }

        void Push(ServerPacket *packet) {
            if (bq != NULL)
                bq->Push(packet);

            else if (q != NULL)
                q->push(packet);
        }

        ServerPacket *Pop() throw(InterruptedException) {
            if (bq != NULL)
                return bq->Pop();

            else if (q != NULL) {
                if (!q->empty()) {
                    ServerPacket *popped = q->front();
                    q->pop();
                    return popped;
                }

                return NULL;
            }

        }

        int Size() {
            if (bq != NULL)
                return bq->Size();

            else if (q != NULL)
                return q->size();
        }
    } commonQueue;

    bool IsPendant(int n) {
        list<int>::iterator it;

        for (it = pendant.begin(); it != pendant.end(); it++)
            if (*it == n) return true;

        return false;
    }

    bool SendPendant(Server *server) {
        do {
            static int size = commonQueue.Size();
            static int counter = 0;

            ServerPacket *packet;

            try {
                packet = commonQueue.Pop();
            } catch (const InterruptedException &ex) {
                cout << ex.what() << endl;
                break;
            }

            if (packet == NULL)
                return false;

            if (IsPendant(packet->GetId())) {                       // If packet ID is on pendant vector
                vector<Address> &clients = server->GetClients();    // Gets dests

                for (int i = 0; i < clients.size(); i++) {
                    packet->GetAddress().Set(clients[i]);           // Set each dest
                    server->Send(packet);                           // Packet is sent
                }

                packet->GetAddress().Clear();                       // Reset address
                commonQueue.Push(packet);                           // Reinsert in queue
            }

            else // avoid memory leak
                delete packet;

            if (!usingThreads && ++counter == size) {
                counter = 0;
                return false;
            }
        } while (server->IsRunning() && usingThreads);

        return true;
    }

    Server::Server(unsigned short port) : DatagramSocket(port) {
        running = true;
    }

    Server::~Server() {
        if (usingThreads) {
            if (!bsqueue.IsInterrupted())
                bsqueue.Interrupt();

            bsqueue.Doit([](ServerPacket *&packet) {
                delete packet;
            });
        }
    }

    void Server::EnableThreadingModel() {
        if (!usingThreads) {
            commonQueue = CommonQueue(&bsqueue);
            usingThreads = true;
            sthread = thread(SendPendant, this);
        }
    }

    vector<Address>& Server::GetClients() {
        return clients;
    }

    bool Server::IsRunning() const {
        return running;
    }

    void Server::Halt() {
        running = false;
    }

    void Server::Mainloop() {
        if (!usingThreads)
            commonQueue = CommonQueue(&squeue);

        while (running) {
            ServerPacket packet;

            if (usingThreads)
                cout << "Waiting for packet... ";

            else
                cout << "Looking for incoming packet...";

            int bytes = Receive(packet);

            if (bytes == 0) {
                cout << "None." << endl;
                continue;
            }

            if (bytes == -1) {
                cout << "Timeout!" << endl;
                continue;
            }

            cout << "Received " << bytes << " bytes." << endl;


            Address &address = packet.GetAddress();

            unsigned int i;
            for (i = 0; i < clients.size(); i++)
                if (clients[i] == address) break;

            if (i == clients.size()) {
                clients.push_back(address);
                cout << "Client connected (" << address.ToString() << ")" << endl;

                ConnectionPacket *connectionPacket = new ConnectionPacket(address, true);
                pendant.push_back(connectionPacket->GetId());
                commonQueue.Push(connectionPacket);
            }


            packet.Process();
            int proto = packet.GetProto();
            int id = packet.GetId();

            switch (proto) {
                case SERVREQ_GET_CONNECTED_CLIENTS: // Clients Request
                    cout << address.ToString() << " requests clients list." << endl;
                    Send(address, ClientsPacket(packet, clients));
                    break;

                case SERVNOTF_ACKNOWLEDGEMENT:
                    cout << "Received notification ACK (" << id << "). ";

                    if (IsPendant(id)) {
                        pendant.remove(id);
                    }

                    else
                        cout << "Dropped.";

                    cout << endl;
                    break;

                default:
                    cout << "Unkonwn protocol. Dropped." << endl;
                    break;
            }

            if (!usingThreads)
                while (SendPendant(this));
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
        server.EnableThreadingModel();

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