#include "RingQueue.hpp"
#include "Task.hpp"
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

std::string SelfName()
{
    char name[128];
    snprintf(name, sizeof name, "0x%x", pthread_self());
    return name;
}

void *ProductorRoutine(void *rq)
{
    // RingQueue<int> *ringqueue = static_cast<RingQueue<int>* >(rq);
    RingQueue<Task> *ringqueue = static_cast<RingQueue<Task> *>(rq);
    while (true)
    {
        sleep(1);
        // version1
        // int data = rand() % 10 + 1;
        // ringqueue->push(data);
        // std::cout << "生产完成，生产的数据是：" << data << std::endl;

        //version2
        //构建or获取任务
        int x = rand() % 10;
        int y = rand() % 15;
        char op = oper[rand() % oper.size()];
        Task t(x, y, op, mymath);
        //生产任务
        ringqueue->push(t);
        //输出提示
        std::cout << SelfName() << " ,生产完成，生产者派发了一个任务" << t.toTaskString() << std::endl;
    }
}

void *ConsumerRoutine(void *rq)
{
    // RingQueue<int> *ringqueue = static_cast<RingQueue<int>* >(rq);
    RingQueue<Task> *ringqueue = static_cast<RingQueue<Task> *>(rq);
    while (true)
    {
        sleep(2);
        // version1
        // int data;
        // ringqueue->Pop(&data);
        // std::cout << "消费完成，消费的数据是：" << data << std::endl;
        
        // version2
        Task t;
        //消费任务
        ringqueue->Pop(&t);
        std::string result = t();
        std::cout << SelfName() << " ,消费者消费了一个任务" << result << std::endl;
    }
}

int main()
{
    srand((unsigned int)time(nullptr) ^ getpid() ^ pthread_self());
    RingQueue<Task> *rq = new RingQueue<Task>();

    pthread_t p[4], c[8];
    for (int i = 0; i < 4;++i)
        pthread_create(p + i, nullptr, ProductorRoutine, rq);
    for (int i = 0; i < 8; ++i)
        pthread_create(c + i, nullptr, ConsumerRoutine, rq);

    for (int i = 0; i < 4; ++i)
        pthread_join(p[i], nullptr);
    for (int i = 0; i < 8; ++i)
        pthread_join(c[i], nullptr);

    delete rq;
    return 0;
}