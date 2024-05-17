#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <functional>
#include <cassert>

namespace ThreadNs
{
    typedef std::function<void *(void *)> func_t;
    const int num = 1024;

    class Thread
    {
    private:
        // 在类内创建线程，想让线程执行对应的方法，需要将方法设置成static
        static void *start_routine(void *args) // 类内成员,有缺失参数this
        {
            Thread *_this = static_cast<Thread *>(args);
            return _this->callback();
        }

    public:
        Thread()
        {
            char buffer[num];
            snprintf(buffer, sizeof buffer, "thread-%d", threadnum++);
            _name = buffer;
        }

        void start(func_t func, void *args = nullptr)
        {
            _func = func;
            _args = args;
            int n = pthread_create(&_tid, nullptr, start_routine, this);
            assert(n == 0);
            (void)n;
        }

        void join()
        {
            int n = pthread_join(_tid, nullptr);
            assert(n == 0);
            (void)n;
        }

        std::string threadname()
        {
            return _name;
        }

        void *callback()
        {
            return _func(_args);
        }

        ~Thread()
        {
            // do nothing
        }

    private:
        std::string _name;
        func_t _func;
        void *_args;
        pthread_t _tid;

        static int threadnum;
    };
    int Thread::threadnum = 1;
}