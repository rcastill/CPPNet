#ifndef NETBYTEBUFFER_H
#define NETBYTEBUFFER_H

#include "core.h"   // ntoh* / hton* functions
#include <cstring>  // memcpy
#include <vector>

using namespace std;

/* Byte buffer designed for network operations. */
class ByteBuffer {
protected:
    char *buffer;           // Base buffer
    int ptr, cap;           // current position, buffer size
    bool shouldClean;       // indicates whether should apply delete[] buffer on destructor
    vector<char *> *copies; // Mantains track of copies if told so

public:
    // Allocates heap for cap bytes
    ByteBuffer(int cap);

    // Passes buffer and its size
    ByteBuffer(void *buffer, int cap);

    // Default ctor. Initializes all variables.
    ByteBuffer();

    /* Applies delete[] on heap if shouldClean
       and on copies if is thy are being tracked */
    virtual ~ByteBuffer();

    /* Transfers buffer and configuration to
       other ByteBuffer (moves data) */
    void Transfer(ByteBuffer *other);
    void Transfer(ByteBuffer &other);

    // shouldClean = false;
    void DisableDestructor();

    // Keeps track of copies
    void TakeCareOfCopies();

    // Gets buffer as void*
    void *GetBuffer() const;

    // A copy of the buffer is returned
    void *GetBufferCopy();

    // Direct reference access to buffer byte
    char &operator[](const unsigned int i);

    /* All Put/Get methods that imply current position
       increments it depending on the data size. And in
       opposite absolute position methods leaves the
       current position untouched */

    // Puts byte on current position
    bool Put(char c);

    // Puts byte on absolute position
    bool Put(char c, int cptr);

    // Puts short on current position
    bool PutShort(short s);

    // Puts short on absolute position
    bool PutShort(short s, int cptr);

    // Puts int on current position
    bool PutInt(int i);

    // Puts int on absolute position
    bool PutInt(int i, int cptr);

    // Puts long on current position
    bool PutLong(long l);

    // Puts long on absolute position
    bool PutLong(long l, int cptr);

    // Puts string on current position
    bool PutString(string);

    // Puts string on absolute position
    bool PutString(string, int);

    // Gets byte from current position
    char Get();

    // Gets byte from absolute position (similar to [])
    char Get(int cptr);

    // Gets short from current position
    short GetShort();

    // Gets short from absolute position
    short GetShort(int cptr);

    // Gets int from current postion
    int GetInt();

    // Gets int from absolute position
    int GetInt(int cptr);

    // Gets long from current position
    long GetLong();

    // Gets long from absolute position
    long GetLong(int cptr);

    // Gets n bytes as string from current position
    string GetString(size_t n);

    // Gets n bytes as string from absolute position
    string GetString(size_t, int);

    // Tells wether <bytes> bytes fit in buffer
    bool Fits(int bytes);

    // Shrinks buffer size to current position
    void ShrinkToFit();

    // Buffer capacity
    int Size() const;

    // ptr getter/setter
    int GetCurrentPosition();
    void SetCurrentPosition(int);

    // Resets current position to 0
    void Rewind();
};

#endif