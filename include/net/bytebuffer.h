#ifndef NETBYTEBUFFER_H
#define NETBYTEBUFFER_H

#include "core.h"
#include <cstring>
#include <vector>

using namespace std;

class ByteBuffer {
protected:
    char *buffer;
    int ptr, cap;
    bool shouldClean;
    vector<char *> *copies;

public:
    ByteBuffer(int);
    ByteBuffer(void *, int);
    ByteBuffer();
    virtual ~ByteBuffer();

    void Transfer(ByteBuffer *);
    void Transfer(ByteBuffer &);
    void DisableDestructor();
    void TakeCareOfCopies();

    void *GetBuffer() const;
    void *GetBufferCopy();
    char &operator[](const unsigned int);

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