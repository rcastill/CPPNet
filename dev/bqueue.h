#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <queue>

#if defined(WIN32)
#include "mingw.thread.h"
#include <mutex>
#include "mingw.mutex.h"
#include "mingw.condition_variable.h"
#elif defined(UNIX)
#include <thread>
#include <mutex>
#include <condition_variable>
#endif

using namespace std;

class InterruptedException : public exception {
public:
    const char *what() const throw() {
        return ".Interrupt was called.";
    }
};

template <class T>
class BlockingQueue {
private:
    queue<T> q;
    mutex mtx;
    condition_variable cv;

    bool interrupted;
public:

    BlockingQueue();

    void Interrupt();
    bool IsInterrupted();
    void Push(const T &);
    T Pop() throw(int);

    void Doit(function<void(T&)> ins);
    void Clear();

    size_t Size() const;
};

template <class T>
BlockingQueue<T>::BlockingQueue() {
    interrupted = false;
}

template <class T>
void BlockingQueue<T>::Interrupt() {
    interrupted = true;
    cv.notify_one();
}

template <class T>
bool BlockingQueue<T>::IsInterrupted() {
    return interrupted;
}

template <class T>
void BlockingQueue<T>::Push(T const &t) {
    {
        unique_lock<mutex> lock(mtx);
        q.push(t);
    }

    cv.notify_one();
}

template <class T>
T BlockingQueue<T>::Pop() throw(int) {
    unique_lock<mutex> lock(mtx);

    while (q.empty()) {
        if (interrupted)
            throw InterruptedException();

        cv.wait(lock);
    }

    T popped = q.front();
    q.pop();

    return popped;
}

template <class T>
size_t BlockingQueue<T>::Size() const {
    return q.size();
}

template <class T>
void BlockingQueue<T>::Doit(function<void(T&)> ins) {
    int size = q.size();

    for (int i = 0; i < size; i++) {
        ins(q.front());
        q.push(q.front());
        q.pop();
    }
}

template <class T>
void BlockingQueue<T>::Clear() {
    while (!q.empty())
        q.pop();
}

#endif