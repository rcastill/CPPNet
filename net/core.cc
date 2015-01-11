#include "core.h"

namespace net {

    bool NetworkInit() {
        #if PLATFORM == PLATFORM_WINDOWS
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR;
        #else
        return true;
        #endif
    }

    void NetworkShutdown() {
        #if PLATFORM == PLATFORM_WINDOWS
        WSACleanup();
        #endif
    }
}