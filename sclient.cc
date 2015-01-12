#include <iostream>
#include "include/net/core.h"
#include "include/net/address.h"
#include "include/net/dgsocket.h"
#include "include/net/packet.h"

using namespace net;

int main(int argc, char **argv) {
    if (NetworkInit()) {
        DatagramSocket datagramSocket(5429);

        if (datagramSocket.IsBound()) {
            cout << "Socket bound at: " << datagramSocket.GetAddress().ToString() << endl;
        }

        datagramSocket.Send(Address(127, 0, 0, 1, 5428), Packet("hola"));
        cout << "Sent" << endl;

        Address address;
        Packet packet;

        datagramSocket.Receive(address, packet);

        cout << "Packet received (" << address.ToString() << ")" <<  endl;
        cout << string(packet.GetData()) << endl;

        NetworkShutdown();
    }

    else
        cout << "Could not init" << endl;
}