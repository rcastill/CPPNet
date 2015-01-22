#ifndef BCLIENT_H
#define BCLIENT_H

#include "dev/spacket.h"
#include "dev/constants.h"
#include <list>

/*  Forward declaration, avoid circular includes.
    #include "server.h" in bclient.cc */
namespace net {
    class Server;
}

using namespace std;
using namespace net;

class BackendClient : public Address {
private:
    Server *server;
    vector<BackendClient> &brothers;
    list<int> pendant;

public:
    BackendClient(Address &address, Server *server);
    BackendClient(Address &address, Server &server);

    void Process(ServerPacket &sp);
    bool IsPendant(int id);
    void SetPendant(int id);
};

#endif // BCLIENT_H