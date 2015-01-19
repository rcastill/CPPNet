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
    bool Put(char, int);
    bool PutShort(short);
    bool PutShort(short, int);
    bool PutInt(int);
    bool PutInt(int, int);
    bool PutLong(long);
    bool PutLong(long, int);
    bool PutString(string);
    bool PutString(string, int);

    char Get();
    char Get(int);
    short GetShort();
    short GetShort(int);
    int GetInt();
    int GetInt(int);
    long GetLong();
    long GetLong(int);
    string GetString(size_t);
    string GetString(size_t, int);

    bool Fits(int);
    void ShrinkToFit();

    int Size() const;
    int Pointer();
    void Pointer(int);
    void Rewind();
};

#endif