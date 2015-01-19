#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <queue>

/* Conccurrent synch tools */
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

/* Java-inspired for handling exception when
   interrupted. */
class InterruptedException : public exception {
public:
    const char *what() const throw() {
        return ".Interrupt was called.";
    }
};

/* Eventually this implementation will be separated in .h/.cc files
   but for now (bussy with something else than damn templates (: )
   it will remain still */

/* Concurrent queue<> implementation. It supports concurrent
   addition of elements, and blocking behavior when popping. */

template <class T>
class BlockingQueue {
private:
    queue<T> q;             // Non-Concurrent queue
    mutex mtx;              // POSIX mutex
    condition_variable cv;  // CV

    bool interrupted;       // Interrupt state
public:

    // Default ctor
    BlockingQueue();

    /* Interrupts concurrent operations. (e.g if popping
       an element and .Interrupt() gets called, a InterruptedException
       is thrown) */
    void Interrupt();

    // Retrieves interrupt status
    bool IsInterrupted();

    // Pushes element to queue
    void Push(const T &);

    // Pops and returns front element from queue
    T Pop() throw(int);

    /* Instruction iterator: ins is called with each
       element as argument */
    void Doit(function<void(T&)> ins);

    // Wipes queue
    void Clear();

    // queue's size
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