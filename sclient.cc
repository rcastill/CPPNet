#include <iostream>
#include "include/net/core.h"
#include "include/net/address.h"
#include "include/net/dgsocket.h"
#include "include/net/packet.h"

using namespace net;

#define TIMEOUT 10

int main(int argc, char **argv) {
    if (NetworkInit()) {
        DatagramSocket datagramSocket(5429);
        //datagramSocket.SetNonBlocking();

        if (datagramSocket.IsBound()) {
            cout << "Socket bound at: " << datagramSocket.GetAddress().ToString() << endl;
        }

        datagramSocket.Send(Address(104, 236, 36, 132, 5428), Packet("Anal"));
        cout << "Sent" << endl;

        Address address;
        Packet packet;

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
            cout << string(packet.GetData()) << endl;
        }

        NetworkShutdown();
    }

    else
        cout << "Could not init" << endl;

    return 0;
}