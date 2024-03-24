#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>

int tickets = 1000;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *start_routine(void *args)
{
    std::string name = static_cast<const char *>(args);
    while(true)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        //判断
        std::cout << name << " -> " << tickets << std::endl;
        tickets--;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    //通过调酒变量控制线程的执行
    pthread_t t[5];
    for (int i = 0; i < 5;++i)
    {
        char *name = new char[64];
        snprintf(name, 64, "thread %d", i + 1);
        pthread_create(t + 1, nullptr, start_routine, name);
    }

    while(true)
    {
        sleep(1);
        // pthread_cond_signal(&cond);
        pthread_cond_broadcast(&cond);
        std::cout << "main thread wakeup one thread..." << std::endl;
    }

    for (int i = 0; i < 5;++i)
    {
        pthread_join(t[i], nullptr);
    }

        return 0;
}
