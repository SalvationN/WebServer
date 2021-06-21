#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "locker.h"

template < typename T >
class threadpool {
public:
    threadpool(int m_thread_number = 8, int m_max_requests = 10000);
    ~threadpool();
    bool append(T* request);
    static void worker(void*);
    void run();

private:
    int m_thread_number;
    int m_max_request;
    pthread_t* m_threads;       // arrays of threads
    std::list<T*> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat;    // if there is any task needed to respond
    bool m_stop;
};

template < typename T >
threadpool<T>::threadpool(int thread_number, int max_requests) :
        m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false), m_threads(NULL)
{
    if(thread_number <= 0 || max_requests <= 0) {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if(!m_threads) {
        throw std::exception();
    }

    for(int i = 0; i < thread_number; ++i) {
        printf("create the %dth thread.\n", i);
        // there is a question: why do this function deliver 'this' to new thread?
        if(pthread_create(m_threads + i, NULL, worker, this) != 0) {
            delete [] m_threads;
            throw std::exception();
        }
        // now thread is unjoinable, main thread don't need to wait for the ending of new thread, new thread will release recourses at
        // the end of execution.
        if(pthread_detach(m_threads[i])) {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template < typename T >
threadpool<T>::~threadpool() {
    delete [] m_threads;
    m_stop = true;
}

template <typename T>
bool threadpool<T>::append() {
    m_queuelocker.lock();
    if(m_workqueue.size() > m_max_requests) {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void threadpool<T>::worker(void* arg) {
    threadpool* pool = (thread* pool) arg;
    pool->run();
    return pool;
}

template <typename T>
void threadpool<T>::run() {
    while( !m_stop ) {
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workqueue.empty()) {
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if(!request) {
            continue;
        }
        request->process();
    }
}
#endif
