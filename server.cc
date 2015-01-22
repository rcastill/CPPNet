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

            vector<BackendClient> &clients = server->GetClients(); // Gets dests
            int pendantCount = 0;

            for (int i = 0; i < clients.size(); i++)
                if (clients[i].IsPendant(packet->GetId())) {
                    packet->GetAddress().Set(clients[i]);
                    server->Send(packet);
                    pendantCount++;
                }


            if (pendantCount) {
                packet->GetAddress().Clear();
                commonQueue.Push(packet);
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

    vector<BackendClient> &Server::GetClients() {
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
            BackendClient *client;

            unsigned int i;
            for (i = 0; i < clients.size(); i++)
                if (clients[i] == address) {
                    client = &client[i];
                    break;
                }

            if (i == clients.size()) {
                clients.push_back(BackendClient(address, this));

                client = &clients.back();

                cout << "Client connected (" << address.ToString() << ")" << endl;

                ConnectionPacket *connectionPacket = new ConnectionPacket(address, true);

                for (i = 0; i < clients.size(); i++)
                    clients[i].SetPendant(connectionPacket->GetId());

                commonQueue.Push(connectionPacket);
            }

            if (!usingThreads)
                while (SendPendant(this));

            client->Process(packet);
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