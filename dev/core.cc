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

    void Wait(float secs) {
        #if PLATFORM == PLATFORM_WINDOWS
        Sleep((DWORD) (secs * 1000.0f));
        #elif PLATFORM == PLATFORM_UNIX
        usleep((int) (secs * 1000000.0f));
        #endif
    }
}