#include <iostream>
#include <chrono>
#include "dev/packet.h"
#include "dev/bqueue.h"

#if defined(WIN32)
#include "dev/mingw.thread.h"
#elif defined(UNIX)
#include <thread>
#endif

using namespace std;
using namespace net;

BlockingQueue<Packet*> bq;
void TestThread() {
    cout << "TestThread()" << endl;

    Packet &packet = *bq.Pop();
    packet.Rewind();
    int popped = packet.GetInt();
    cout << "Popped: " << popped << endl;
}

inline void Wait(int ms) {
    cout << "Wait(" << ms << ")" << endl;
    this_thread::sleep_for(chrono::milliseconds(ms));
}

int main(int argc, char **argv) {
    thread t(TestThread);
    Packet packet(sizeof(int));
    packet.PutInt(42);
    Wait(5000);
    bq.Push(&packet);
    t.join();
    return 0;
}