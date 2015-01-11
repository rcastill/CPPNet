#include <iostream>
#include "include/net/core.h"
#include "include/net/address.h"
#include "include/net/dgsocket.h"
#include "include/net/packet.h"

using namespace net;
using namespace std;

int main(int argc, char **argv) {
    if (NetworkInit()) {
        DatagramSocket datagramSocket(5428);

        Address sender;
        Packet packet;

        datagramSocket.Receive(sender, packet);

        std::string str = std::string(packet.GetData());

        cout << "Packet received from: " << sender.ToString() << endl;
        cout << str << endl;

        NetworkShutdown();
    }

    else
        cout << "Could not init" << endl;
}