#include "bytebuffer.h"


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
            delete[] copies->at((size_t) i);
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

void ByteBuffer::SetCurrentPosition(int ptr) {
    if (ptr < cap && ptr >= 0)
        this->ptr = ptr;
}

int ByteBuffer::GetCurrentPosition() {
    return ptr;
}

void ByteBuffer::Rewind() {
    ptr = 0;
}

bool ByteBuffer::Put(char c) {
    if (ptr < cap) {
        buffer[ptr++] = c;
        return true;
    }

    return false;
}

bool ByteBuffer::Put(char c, int cptr) {
    if (cptr < cap) {
        buffer[cptr] = c;
        return true;
    }

    return false;
}

bool ByteBuffer::PutShort(short s) {
    return PutNonFloating<short>(this, (short) htons((u_short) s));
}

bool ByteBuffer::PutShort(short s, int cptr) {
    int lptr = ptr;
    ptr = cptr;
    bool ans = PutShort(s);
    ptr = lptr;
    return ans;
}

bool ByteBuffer::PutInt(int i) {
    return PutNonFloating<int>(this, (int) htonl((u_long) i));
}

bool ByteBuffer::PutInt(int i, int cptr) {
    int lptr = ptr;
    ptr = cptr;
    bool ans = PutInt(i);
    ptr = lptr;
    return ans;
}

bool ByteBuffer::PutLong(long l) {
    return PutNonFloating<long>(this, htonl((u_long) l));
}

bool ByteBuffer::PutLong(long l, int cptr) {
    int lptr = ptr;
    ptr = cptr;
    bool ans = PutLong(l);
    ptr = lptr;
    return ans;
}

bool ByteBuffer::PutString(string str) {
    size_t length = str.length();

    if (Fits(length)) {
        str.copy(&buffer[ptr], length, 0);
        ptr += length + 1;
        return true;
    }

    return false;
}

bool ByteBuffer::PutString(string str, int cptr) {
    int lptr = ptr;
    ptr = cptr;
    bool ans = PutString(str);
    ptr = lptr;
    return ans;
}

char ByteBuffer::Get() {
    return buffer[ptr++];
}

char ByteBuffer::Get(int cptr) {
    return buffer[cptr];
}

short ByteBuffer::GetShort() {
    return (short) ntohs((u_short) GetNonFloating<short>(this));
}

short ByteBuffer::GetShort(int cptr) {
    int lptr = ptr;
    ptr = cptr;
    short ans = GetShort();
    ptr = lptr;
    return ans;
}

int ByteBuffer::GetInt() {
    return (int) ntohl((u_long) GetNonFloating<int>(this));
}

int ByteBuffer::GetInt(int cptr) {
    int lptr = ptr;
    ptr = cptr;
    int ans = GetInt();
    ptr = lptr;
    return ans;
}

long ByteBuffer::GetLong() {
    return (long) ntohl((u_long) GetNonFloating<long>(this));
}

long ByteBuffer::GetLong(int cptr) {
    int lptr = ptr;
    ptr = cptr;
    long ans = GetLong();
    ptr = lptr;
    return ans;
}

string ByteBuffer::GetString(size_t length) {
    string str = string(&buffer[ptr], length);
    ptr += length + 1;
    return str;
}

string ByteBuffer::GetString(size_t length, int cptr) {
    int lptr = ptr;
    ptr = cptr;
    string ans = GetString(length);
    ptr = lptr;
    return ans;
}

bool ByteBuffer::Fits(int bytes) {
    return ptr + bytes <= cap;
}

void ByteBuffer::ShrinkToFit() {
    cap = ptr + 1;
    char *newHome = new char[cap];
    memcpy(newHome, buffer, (size_t) cap);

    if (shouldClean)
        delete[] buffer;

    shouldClean = true;
    buffer = newHome;
}

int ByteBuffer::Size() const {
    return cap;
}

template <class T>
bool PutNonFloating(ByteBuffer *bb, T t) {
    int bytes = sizeof(T);

    if (!bb->Fits(bytes))
        return false;

    // little endian
    for (int i = 0; i < bytes; i++)
        bb->Put((char) ((t >> 8 * i) & 0xff));

    // big endian
    /*for (int i = bytes - 1; i >= 0; i--)
        bb->Put((char) ((t >> 8 * i) & 0xff)); // CLEAN EVERYTHING*/

    return true;
}

template <class T>
T GetNonFloating(ByteBuffer *bb) {
    int bytes = sizeof(T);
    T t = 0; // PLEASE DO NEVER FORGET TO INITIALIZE. THANKS.

    // little endian
    for (int i = 0; i < bytes; i++)
        t |= (((T) bb->Get()) & 0xff) << 8 * i;

    // big endian
    /*for (int i = bytes - 1; i >= 0; i--)
        t |= (((T) bb->Get()) & 0xff) << 8 * i; // EVEN CASTS! */

    return t;
}
