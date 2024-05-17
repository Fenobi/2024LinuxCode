#pragma once

#include "Thread.hpp"
#include "LockGuars.hpp"
#include "log.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <pthread.h>
#include <unistd.h>

using namespace ThreadNs;

const int gnum = 10;

template <class T>
class ThreadPool;

template <class T>
class ThreadData
{
public:
    ThreadPool<T> *threadpool;
    std::string name;

public:
    ThreadData(ThreadPool<T> *tp, const std::string n) : threadpool(tp), name(n)
    {
    }
};

template <class T>
class ThreadPool
{
private:
    static void *handlerTask(void *args)
    {
        ThreadData<T> *td = (ThreadData<T> *)args;
        while (true)
        {
            T t;
            {
                LockGuard lockguard(td->threadpool->mutex());
                while (td->threadpool->isQueueEmpty())
                {
                    td->threadpool->threadWait();
                }
                t = td->threadpool->pop(); // pop的本质，是将任务从公共队列中，拿到当前线程自己独立的栈中
            }
            t();
        }
        delete td;
        return nullptr;
    }

    ThreadPool(const int &num = gnum) : _num(num)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
        for (int i = 0; i < num; ++i)
        {
            _threads.push_back(new Thread());
        }
    }
    void operator=(const ThreadPool<T> *pt) = delete;
    ThreadPool(const ThreadPool<T> &pt) = delete;

public:
    void lockQueue() { pthread_mutex_lock(&_mutex); }
    void unlockQueue() { pthread_mutex_unlock(&_mutex); }
    bool isQueueEmpty() { return _task_queue.empty(); }
    void threadWait() { pthread_cond_wait(&_cond, &_mutex); }
    pthread_mutex_t *mutex() { return &_mutex; }

    T pop()
    {
        T t = _task_queue.front();
        _task_queue.pop();
        return t;
    }

public:
    void run()
    {
        for (const auto &t : _threads)
        {
            ThreadData<T> *td = new ThreadData<T>(this, t->threadname());

            t->start(handlerTask, td);
            //std::cout << t->threadname() << " start..." << std::endl;
            logMessage(DEBUG, "%s start...", t->threadname().c_str());

        }
    }
    void push(const T &in)
    {
        // pthread_mutex_lock(&_mutex);
        // _task_queue.push(in);
        // pthread_cond_signal(&_cond);
        // pthread_mutex_unlock(&_mutex);
        LockGuard lockguard(&_mutex);
        _task_queue.push(in);
        pthread_cond_signal(&_cond);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
        for (const auto &t : _threads)
            delete t;
    }

    // 使用单例模式
    static ThreadPool<T> *getInstance()
    {
        if(nullptr == tp)
        {
            _singlelock.lock();
            if (nullptr == tp)
            {
                tp = new ThreadPool<T>();
            }
            _singlelock.unlock();
        }
        return tp;
    }

private:
    int _num;
    std::vector<Thread *> _threads;
    std::queue<T> _task_queue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;

    static ThreadPool<T> *tp;
    static std::mutex _singlelock;
};

template <class T>
ThreadPool<T> *ThreadPool<T>::tp = nullptr;

template <class T>
std::mutex ThreadPool<T>::_singlelock;