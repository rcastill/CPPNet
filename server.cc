// TODO ACK CLIENT DEPENDENT

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

    //list<int> pendant;
    map<int, int> pendantMAP;

    thread sthread;

    class ACKManager : public Address {
    private:
        list<int> ignore;

        bool ShouldIgnore(int ack) {
            list<int>::iterator it;

            for (it = ignore.begin(); it != ignore.end(); it++)
                if (ack == *it) return true;

            return false;
        }

    public:
        ACKManager(const Address &other) : Address(other) {}

        void ProcessACK(int ack) {
            if (!ShouldIgnore(ack)) {
                cout << "ACK being processed by " << ToString() << endl;
                ignore.push_back(ack);

                cout << "Current counter: " << pendantMAP[ack] << endl;

                if (--pendantMAP[ack] == 0)
                    ignore.remove(ack);

                cout << "New value: " << pendantMAP[ack] << endl;
            }
        }
    };

    vector<ACKManager> clients;

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

            return NULL;
        }

        size_t Size() {
            if (bq != NULL)
                return bq->Size();

            else if (q != NULL)
                return q->size();

            return 0;
        }
    } commonQueue;

    /*bool IsPendant(int n) {
        list<int>::iterator it;

        for (it = pendant.begin(); it != pendant.end(); it++)
            if (*it == n) return true;

        return false;
    }*/

    bool IsPendantMAP(int n) {
        map<int, int>::iterator it;

        for (it = pendantMAP.begin(); it != pendantMAP.end(); it++)
            if (it->first == n) return true;

        return false;
    }

    void IncrementPendantCounters() {
        map<int, int>::iterator it;

        for (it = pendantMAP.begin(); it != pendantMAP.end(); it++)
            it->second++;
    }

    bool SendPendant(Server *server) {
        do {
            static size_t size = commonQueue.Size();
            static size_t counter = 0;

            ServerPacket *packet;

            try {
                packet = commonQueue.Pop();
            } catch (const InterruptedException &ex) {
                cout << ex.what() << endl;
                break;
            }

            if (packet == NULL)
                return false;

            if (IsPendantMAP(packet->GetId())) {                       // If packet ID is on pendant vector
                //vector<Address> &clients = server->GetClients();    // Gets dests

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

    /*vector<Address>& Server::GetClients() {
        return clients;
    }*/

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


            //Address &address = packet.GetAddress();
            //ACKManager ackManager(address);
            ACKManager client(packet.GetAddress());

            unsigned int i;
            for (i = 0; i < clients.size(); i++)
                if (clients[i] == client) break;

            if (i == clients.size()) {
                clients.push_back(client);
                //clients.push_back(ackManager);
                cout << "Client connected (" << client.ToString() << ")" << endl;

                IncrementPendantCounters();

                ConnectionPacket *connectionPacket = new ConnectionPacket(client, true);
                //pendant.push_back(connectionPacket->GetId());
                pendantMAP[connectionPacket->GetId()] = (int) clients.size();
                cout << "Set as pendant ID=" << connectionPacket->GetId() << " with counter=" << clients.size() << "." << endl;
                commonQueue.Push(connectionPacket);
            }

            packet.Process();
            int proto = packet.GetProto();
            int id = packet.GetId();

            switch (proto) {
                case SERVREQ_GET_CONNECTED_CLIENTS: // Clients Request
                    cout << client.ToString() << " requests clients list." << endl;
                    Send(client, ClientsPacket(packet, &clients[0], sizeof(ACKManager), clients.size()));
                    break;

                case SERVNOTF_ACKNOWLEDGEMENT:
                    cout << "Received notification ACK (" << id << "). ";

                    if (IsPendantMAP(id)) {
                        for (i = 0; i < clients.size(); i++) {
                            if (clients[i] == client) {
                                ACKManager &m = clients[i];
                                m.ProcessACK(id);
                                break;
                            }
                        }

                        cout << "Could not find client." << endl;
                    }

                    else
                        cout << "Dropped.";

                    //if (IsPendant(id))
                    //    pendant.remove(id);

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
        //server.EnableThreadingModel();

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