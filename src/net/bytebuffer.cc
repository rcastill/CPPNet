#include "../../include/net/bytebuffer.h"


template <class T> bool PutNonFloating(ByteBuffer *, T);
template <class T> T GetNonFloating(ByteBuffer *);

ByteBuffer::ByteBuffer(void *buffer, int cap) {
    this->buffer = (char *) buffer;
    //memset(buffer, 0, (size_t) cap);
    this->cap = cap;
    ptr = 0;
    shouldClean = true;
    copies = NULL;
}

ByteBuffer::ByteBuffer(int cap) : ByteBuffer(new char[cap], cap) {
    memset(buffer, 0, (size_t) cap);
}

/**
* Constructor designed to be transfered in
*/
ByteBuffer::ByteBuffer() {
    buffer = NULL;
    cap = 0;
    ptr = 0;
    shouldClean = false;
    copies = NULL;
}

ByteBuffer::~ByteBuffer() {
    if (shouldClean)
        delete[] buffer;

    if (copies != NULL)
        for (int i = 0; i < copies->size(); i++)
            delete[] (*copies)[i];
}

void ByteBuffer::Transfer(ByteBuffer *other) {
    other->buffer = buffer;
    other->ptr = ptr;
    other->cap = cap;
    other->shouldClean = shouldClean;
    other->copies = copies;

    DisableDestructor();
    buffer = NULL;
    copies = NULL;
}

void ByteBuffer::Transfer(ByteBuffer &other) {
    Transfer(&other);
}

void *ByteBuffer::GetBuffer() const {
    return buffer;
}

void *ByteBuffer::GetBufferCopy() {
    char *copy = new char[cap];
    memcpy(copy, buffer, (size_t) cap);

    if (copies != NULL)
        copies->push_back(copy);

    return copy;
}

char &ByteBuffer::operator[](unsigned int i) {
    return buffer[i];
}

void ByteBuffer::DisableDestructor() {
    shouldClean = false;
}

void ByteBuffer::TakeCareOfCopies() {
    copies = new vector<char *>;
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
    if (ptr < cap) {
        buffer[ptr++] = c;
        return true;
    }

    return false;
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
    return buffer[ptr++];
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
    int bytes = sizeof(T);

    if (bb->Pointer() + bytes > bb->Size())
        return false;

    // little endian
    /*for (int i = 0; i < bytes; i++) {
        buffer[bb->Pointer()] = (char) (t >> 8 * i);
        bb->Pointer(bb->Pointer() + 1);
    }*/

    // big endian
    for (int i = bytes - 1; i >= 0; i--)
        bb->Put((char) ((t >> 8 * i) & 0xff)); // CLEAN EVERYTHING

    return true;
}

template <class T>
T GetNonFloating(ByteBuffer *bb) {
    int bytes = sizeof(T);
    T t = 0; // PLEASE DO NEVER FORGET TO INITIALIZE. THANKS.

    // little endian
    /*for (int i = 0; i < bytes; i++) {
        t |= (((T) buffer[bb->Pointer()]) << 8 * i);
        bb->Pointer(bb->Pointer() + 1);
    }*/

    // big endian
    for (int i = bytes - 1; i >= 0; i--)
        t |= (((T) bb->Get()) & 0xff) << 8 * i; // EVEN CASTS!

    return t;
}
