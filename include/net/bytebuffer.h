#ifndef NETBYTEBUFFER_H
#define NETBYTEBUFFER_H

#include "core.h"
#include <cstring>
#include <vector>

using namespace std;

class ByteBuffer {
private:
    char *buffer;
    int ptr, cap;
    bool shouldClean, shouldTrackCopies;
    vector<char *> copies;

public:
    ByteBuffer(int);
    ByteBuffer(void *, int);
    ~ByteBuffer();

    void DisableDestructor();
    void TakeCareOfCopies();

    void *GetBuffer();
    void *GetBufferCopy();

    bool Put(char);
    bool PutShort(short);
    bool PutInt(int);
    bool PutLong(long);

    char Get();
    short GetShort();
    int GetInt();
    long GetLong();

    int Size() const;
    int Pointer();
    void Pointer(int);
    void Rewind();
};

#endif