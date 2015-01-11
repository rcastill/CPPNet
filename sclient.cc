#include <iostream>
#include "include/net/core.h"
#include "include/net/address.h"
#include "include/net/dgsocket.h"
#include "include/net/packet.h"

using namespace net;

int main(int argc, char **argv) {
    if (NetworkInit()) {
        DatagramSocket datagramSocket;
        datagramSocket.Send(Address(104, 131, 173, 250, 5428), Packet("hola"));
        cout << "Sent" << endl;
        NetworkShutdown();
    }

    else
        cout << "Could not init" << endl;
}