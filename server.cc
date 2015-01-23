// TODO ACK CLIENT DEPENDENT

#include "server.h"

#if PLATFORM == PLATFORM_WINDOWS
#include "dev/mingw.thread.h"
#elif PLATFORM == PLATFORM_UNIX
#include <thread>
#endif

/*
* Hybrid queue wrapper. It could be a blocking queue
* or a standart queue. It will be set depending on the
* enabled server model.
*/
struct CommonQueue {
private:
    BlockingQueue<ServerPacket*> *bq;   // BlockingQueue slot
    queue<ServerPacket*> *q;            // std::queue slot

public:
    // Default ctor
    CommonQueue() {
        bq = NULL;
        q = NULL;
    }

    // Uses BlockingQueue
    CommonQueue(BlockingQueue<ServerPacket*> *kiwi) {
        bq = kiwi;
        q = NULL;
    }

    // Uses std::queue
    CommonQueue(queue<ServerPacket*> *kiwi) {
        bq = NULL;
        q = kiwi;
    }

    // Moves pointers
    CommonQueue &operator=(CommonQueue&& other) {
        bq = other.bq;
        q = other.q;

        other.bq = NULL;
        other.q = NULL;

        return *this;
    }

    // Pushes to selected queue
    void Push(ServerPacket *packet) {
        if (bq != NULL)
            bq->Push(packet);

        else if (q != NULL)
            q->push(packet);
    }

    // Pops from selected queue
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

    // Gets selected queue's size
    size_t Size() {
        if (bq != NULL)
            return bq->Size();

        else if (q != NULL)
            return q->size();

        return 0;
    }
} commonQueue;

namespace net {
    bool usingThreads = false;              // Whether or not the threading model is being used
    BlockingQueue<ServerPacket*> bsqueue;   // BlockingQueue (Threading model use)
    queue<ServerPacket*> squeue;            // std::queue (else)

    thread sthread;                         // Send thread (Threading model use)
    long sendingInterval = 0;               // Sending interval (ms)

    /*
    * Sends packets in the send queue (the selected one) to
    * the clients that did not received an ACK yet. It works
    * for threading model and else.
    *
    * If threading model is being used, this function loops
    * until the server is shutdown (blocks when there are not
    * packets to send).
    *
    * If else, this function returns true when there are more
    * packets to be sent (if the queue has packets, this function
    * returns false when it is called n times, where n is the size
    * stored the first time it was called).
    */
    bool SendPendant(Server *server) {
        do {
            static size_t size = commonQueue.Size();
            static size_t counter = 0;

            if (usingThreads)   // It's nap time!!!
                this_thread::sleep_for(chrono::milliseconds(sendingInterval));

            ServerPacket *packet;

            try {
                packet = commonQueue.Pop();
            } catch (const InterruptedException &ex) {
                // Threading model, and it was interrupted
                cout << ex.what() << endl;
                break;
            }

            // else model, and there is no packet to be sent
            if (packet == NULL)
                return false;

            vector<BackendClient> &clients = server->GetClients();  // Gets dests
            int pendantCount = 0;                                   // Pendant clients for this packet

            for (int i = 0; i < clients.size(); i++)                // For each client
                if (clients[i].IsPendant(packet->GetId())) {        // if this packet was not ack'd
                    packet->GetAddress().Set(clients[i]);
                    server->Send(packet);                           // send it

                    cout << "Queued packet sent (" << packet->GetId() << ")" << endl;

                    pendantCount++;                                 // Client pendant
                }


            if (pendantCount) { // If there where pendant we need the packet back in the queue
                packet->GetAddress().Clear();
                commonQueue.Push(packet);
            }

            else // avoid memory leak (no client reported pendant i.e. every client received it)
                delete packet;

            if (!usingThreads && ++counter == size) {   // else model, if iterated through all packets. finish.
                counter = 0;
                return false;
            }

        } while (server->IsRunning() && usingThreads);  // if in threading model, this is a loop :)

        return true;    // else model, there are more packets to revise
    }

    Server::Server(unsigned short port) : DatagramSocket(port) {
        running = true;
    }

    Server::~Server() {
        if (usingThreads) {
            if (!bsqueue.IsInterrupted())
                bsqueue.Interrupt();

            // Do release heap for code's sake
            bsqueue.Doit([](ServerPacket *&packet) {
                delete packet;
            });
        }
    }

    void Server::EnableThreadingModel() {
        if (!usingThreads) {
            commonQueue = CommonQueue(&bsqueue);    // Select BlockingQueue
            usingThreads = true;
            sthread = thread(SendPendant, this);    // Spawn thread
        }
    }

    void Server::SetSendingInterval(long ms) {
        sendingInterval = ms;
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
        if (!usingThreads)                      // Threading model was not enabled
            commonQueue = CommonQueue(&squeue); // Select std::queue

        while (running) {
            ServerPacket packet;

            cout << "Waiting for packet... ";

            int bytes = Receive(packet);    // Receives packet

            if (bytes == 0) // NonBlocking socket and nothing received this iteration
                cout << "None." << endl;

            else if (bytes == -1) // Timeout set > 0 and timed out
                cout << "Timeout!" << endl;

            else {
                cout << "Received " << bytes << " bytes." << endl;

                Address &address = packet.GetAddress();
                BackendClient *client = NULL;

                unsigned int i;
                for (i = 0; i < clients.size(); i++)
                    if (clients[i] == address) {
                        client = &clients[i];
                        break;
                    }

                if (i == clients.size()) {  // New connection
                    clients.push_back(BackendClient(address, this));    // register in clients coll

                    client = &clients.back();

                    cout << "Client connected (" << address.ToString() << ")" << endl;

                    ConnectionPacket *connectionPacket = new ConnectionPacket(address, true);   // Notification

                    for (i = 0; i < clients.size(); i++)    // Every client has a pendant notification
                        clients[i].SetPendant(connectionPacket->GetId());

                    commonQueue.Push(connectionPacket); // Put notification in selected queue
                }

                if (client != NULL)
                    client->Process(packet);

                else    // Theorically impossible but still :3
                    cout << "Paradox?" << endl;
            }

            if (!usingThreads)  // else model, send packets in queue to those who are still pendant
                while (SendPendant(this));

        } // while (running)

    } // void Server::Mainloop()

} // namespace net

using namespace net;

int main(int argc, char **argv) {
    unsigned short port = 50000;        // default port
    long timeout = 0;                   // default timeout
    int shouldUseThreadingModel = 0;    // default model

    /* Assume right input */

    if (argc == 2)
        port = (unsigned short) atoi(argv[1]);

    if (argc == 3) {
        port = (unsigned short) atoi(argv[1]);
        timeout = atol(argv[2]);
    }

    if (argc == 4) {
        port = (unsigned short) atoi(argv[1]);
        timeout = atol(argv[2]);
        shouldUseThreadingModel = atoi(argv[3]);
    }

    if (NetworkInit()) {
        Server server(port);
        cout << "Threading model ";

        if (shouldUseThreadingModel) {
            cout << "enabled";
            server.EnableThreadingModel();  // Use threading model
            server.SetSendingInterval(100); // Everybody needs to sleep
        }

        else
            cout << "disabled";

        cout << endl;

        if (server.IsValid() && server.IsBound()) {
            cout << "Server bound @ " << server.GetAddress().ToString() << endl;
            cout << "Receive timeout set to " << timeout << endl;

            if (timeout != 0)   // with timeout > 0, socket must be set non blocking
                server.SetNonBlocking();

            server.SetTimeout(timeout);
            server.Mainloop();
            NetworkShutdown();  // Go windows!
        }

        cout << "Not valid or bound" << endl;
    }

    else
        cout << "Could not initialize network" << endl;

    return 0;
}