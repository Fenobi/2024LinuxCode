#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <semaphore.h>
#include <pthread.h>

static const int gcap = 10;

template<class T>
class RingQueue
{
private:
    void P(sem_t &sem)
    {
        int n = sem_wait(&sem);
        assert(n == 0);
        (void)n;
    }
    void V(sem_t &sem)
    {
        int n = sem_post(&sem);
        assert(n == 0);
        (void)n;
    }

public:
    RingQueue(const int &cap = gcap) : _queue(cap), _cap(cap)
    {
        int n = sem_init(&_spaceSem, 0, _cap);
        assert(n == 0);
        n = sem_init(&_dataSem, 0, 0);
        assert(n == 0);

        _producerStep = _consumerStep = 0;

        pthread_mutex_init(&_pmutex, nullptr);
        pthread_mutex_init(&_cmutex, nullptr);
    }
    // 生产者
    void push(const T &in)
    {
        //先加锁还是先申请信号量？
        // pthread_mutex_lock(&_pmutex);
        // P(_spaceSem); // 申请到了空间信号量，意味着我们一定能进行正常的生产
        
        P(_spaceSem); // 申请到了空间信号量，意味着我们一定能进行正常的生产
        pthread_mutex_lock(&_pmutex);

        _queue[_producerStep++] = in;
        _producerStep %= _cap;
        
        pthread_mutex_unlock(&_pmutex);
        V(_dataSem);
        // V(_dataSem);
        // pthread_mutex_unlock(&_pmutex);
    }
    //消费者
    void Pop(T *out)
    {
        //消费者同上
        P(_dataSem);
        pthread_mutex_lock(&_cmutex);
        
        *out = _queue[_consumerStep++];
        _consumerStep %= _cap;
        
        pthread_mutex_unlock(&_cmutex);
        V(_spaceSem);
    }
    ~RingQueue()
    {
        sem_destroy(&_spaceSem);
        sem_destroy(&_dataSem);

        pthread_mutex_destroy(&_pmutex);
        pthread_mutex_destroy(&_cmutex);
    }

private:
    std::vector<T> _queue;
    int _cap;
    sem_t _spaceSem;//生产者 想生产，看中的是空间资源
    sem_t _dataSem;//消费者 想消费，看中的是数据资源
    int _producerStep;
    int _consumerStep;
    pthread_mutex_t _pmutex;
    pthread_mutex_t _cmutex;
};