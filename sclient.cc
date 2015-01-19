#include <iostream>
#include "dev/core.h"
#include "dev/address.h"
#include "dev/dgsocket.h"
#include "dev/cpacket.h"
#include "dev/connpacket.h"
#include "dev/constants.h"
#include "dev/packet.h"
#include <vector>

using namespace net;

#define TIMEOUT 10
#define RUN_N_TIMES 10

vector<int> ignored;

bool ShouldIgnore(int i) {
    for (int j = 0; j < ignored.size(); j++)
        if (ignored[j] == i)
            return true;

    return false;
}

int main(int argc, char **argv) {

    if (NetworkInit()) {
        DatagramSocket datagramSocket(50000);

        if (datagramSocket.IsBound()) {
            cout << "Socket bound at: " << datagramSocket.GetAddress().ToString() << endl;
        }

        datagramSocket.Send(Address(104, 236, 36, 132, 5428), ClientsPacket());

        for (int i = 0; i < RUN_N_TIMES; i++) {
            Address address;
            ServerPacket packet;

            datagramSocket.Receive(address, packet);

            packet.Process();

            int proto = packet.GetProto();
            int id = packet.GetId();

            switch (proto) {
                case SERVREQ_GET_CONNECTED_CLIENTS: {
                    cout << "Received answer (" << id << ")" << endl;
                    ClientsPacket clientsPacket(packet);
                    cout << "clientsPacket.Process();" << endl;
                    clientsPacket.Process();

                    cout << clientsPacket.ToString() << endl;

                    break;
                }

                case SERVNOTF_CLIENT_CONNECTED:
                case SERVNOTF_CLIENT_DISCONNECTED: {
                    if (!ShouldIgnore(id)) {
                        ConnectionPacket connectionPacket(packet);
                        cout << connectionPacket.ToString() << endl;
                        ServerPacket ack(SERVNOTF_ACKNOWLEDGEMENT, id);
                        ack.GetAddress().Set(127, 0, 0, 1, 5428);
                        datagramSocket.Send(ack);
                        ignored.push_back(id);
                        break;
                    }

                    else
                        cout << "Ignoring " << id << "." << endl;
                }

                default:
                    cout << "Unknown protocol" << endl;
                    break;
            }
        }

        /*datagramSocket.Send(Address(104, 236, 36, 132, 5428), Packet("Anal"));
        cout << "Sent" << endl;

        Address address;
        Packet packet(BUFFER_SIZE);

        fd_set rfds;
        timeval tv;
        FD_ZERO(&rfds);
        FD_SET(datagramSocket.GetSocket(), &rfds);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

        int retval = select(1, &rfds, NULL, NULL, &tv);


        if (retval == 0) {
            cout << "Receive timed out (" << TIMEOUT << ")" << endl;
        }

        else {
            int bytes = datagramSocket.Receive(address, packet);
            cout << "Packet received (" << bytes << " bytes) from " << address.ToString() << endl;
            cout << string((char *) packet.GetBuffer()) << endl;
        }*/

        NetworkShutdown();
    }

    else
        cout << "Could not init" << endl;

    return 0;
}