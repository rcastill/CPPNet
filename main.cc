#include "net/core.h"
#include "net/dgsocket.h"

using namespace std;

int main() {
    net::NetworkInit();

    net::DatagramSocket datagramSocket;
    if (!datagramSocket.IsValid())
        cout << "Not valid" << endl;

    if (!datagramSocket.IsBound())
        cout << "Not bound" << endl;

    else
        cout << "Address: " << datagramSocket.GetAddress().ToString() << endl;

    if (!datagramSocket.SetNonBlocking())
        cout << "Could not set non blocking" << endl;

    net::NetworkShutdown();
    return 0;
}