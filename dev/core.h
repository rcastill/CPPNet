#ifndef NETCORE_H
#define NETCORE_H

#define PLATFORM_WINDOWS    0
#define PLATFORM_UNIX       1

#if defined(WIN32)
#define PLATFORM            PLATFORM_WINDOWS
#else
#define PLATFORM            PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
#include <winsock2.h>
typedef int socklen_t;
#pragma comment(lib, "wsock32.lib")
#elif PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#endif

const int BUFFER_SIZE = 256;    // MAX_PACKET_SIZE

#include <iostream>
using namespace std;

namespace net {
    bool NetworkInit();     // Inits WSA (Windows only, in linux it does nothing)
    void NetworkShutdown(); // Shuts down WSA (Windows onlu, in linux it does nothing)
    void Wait(float s);     // Waits for s seconds
}

#endif