#include "../../include/net/bytebuffer.h"


template <class T> bool PutNonFloating(ByteBuffer *, T);
template <class T> T GetNonFloating(ByteBuffer *);

ByteBuffer::ByteBuffer(void *buffer, int cap) {
    this->buffer = (char *) buffer;
    //memset(buffer, 0, cap);
    this->cap = cap;
    ptr = 0;
    shouldClean = true;
    shouldTrackCopies = false;
}

ByteBuffer::ByteBuffer(int cap) : ByteBuffer(new char[cap], cap) {}

ByteBuffer::~ByteBuffer() {
    if (shouldClean)
        delete[] buffer;

    if (shouldTrackCopies)
        for (int i = 0; i < copies.size(); i++)
            delete[] copies[i];
}

void *ByteBuffer::GetBuffer() {
    return buffer;
}

void *ByteBuffer::GetBufferCopy() {
    char *copy = new char[cap];
    memcpy(copy, buffer, (size_t) cap);

    if (shouldTrackCopies)
        copies.push_back(copy);

    return copy;
}

void ByteBuffer::DisableDestructor() {
    shouldClean = false;
}

void ByteBuffer::TakeCareOfCopies() {
    shouldTrackCopies = true;
}

void ByteBuffer::Pointer(int ptr) {
    if (ptr < cap && ptr >= 0)
        this->ptr = ptr;
}

int ByteBuffer::Pointer() {
    return ptr;
}

void ByteBuffer::Rewind() {
    Pointer(0);
}

bool ByteBuffer::Put(char c) {
    return PutNonFloating<char>(this, c);
}

bool ByteBuffer::PutShort(short s) {
    return PutNonFloating<short>(this, (short) htons((u_short) s));
}

bool ByteBuffer::PutInt(int i) {
    return PutNonFloating<int>(this, (int) htonl((u_long) i));
}

bool ByteBuffer::PutLong(long l) {
    return PutNonFloating<long>(this, htonl((u_long) l));
}

char ByteBuffer::Get() {
    return GetNonFloating<char>(this);
}

short ByteBuffer::GetShort() {
    return (short) ntohs((u_short) GetNonFloating<short>(this));
}

int ByteBuffer::GetInt() {
    return (int) ntohl((u_long) GetNonFloating<int>(this));
}

long ByteBuffer::GetLong() {
    return (long) ntohl((u_long) GetNonFloating<long>(this));
}

int ByteBuffer::Size() const {
    return cap;
}

template <class T>
bool PutNonFloating(ByteBuffer *bb, T t) {
    int BYTES = sizeof(T);
    char *buffer = (char *) bb->GetBuffer();

    if (bb->Pointer() + BYTES > bb->Size())
        return false;

    // little endian
    /*for (int i = 0; i < BYTES; i++) {
        buffer[bb->Pointer()] = (char) (t >> 8 * i);
        bb->Pointer(bb->Pointer() + 1);
    }*/

    // big endian
    for (int i = BYTES - 1; i >= 0; i--) {
        buffer[bb->Pointer()] = (char) (t >> 8 * i);
        bb->Pointer(bb->Pointer() + 1);
    }

    return true;
}

template <class T>
T GetNonFloating(ByteBuffer *bb) {
    int BYTES = sizeof(T);
    char *buffer = (char *) bb->GetBuffer();
    T t = 0; // PLEASE DO NEVER FORGET TO INITIALIZE. THANKS.

    // little endian
    /*for (int i = 0; i < BYTES; i++) {
        t |= (((T) buffer[bb->Pointer()]) << 8 * i);
        bb->Pointer(bb->Pointer() + 1);
    }*/

    // big endian
    for (int i = BYTES - 1; i >= 0; i--) {
        t |= (((T) buffer[bb->Pointer()]) << 8 * i);
        bb->Pointer(bb->Pointer() + 1);
    }

    return t;
}
