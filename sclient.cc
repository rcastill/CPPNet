#include <iostream>
#include "net/core.h"
#include "net/address.h"
#include "net/dgsocket.h"
#include "net/packet.h"

using namespace net;

int main(int argc, char **argv) {
    if (NetworkInit()) {
        DatagramSocket datagramSocket;
        datagramSocket.Send(Address(127, 0, 0, 1, 5428), Packet("hola"));
        cout << "Sent" << endl;
        NetworkShutdown();
    }

    else
        cout << "Could not init" << endl;
}