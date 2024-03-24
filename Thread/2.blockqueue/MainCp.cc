#include "BlockQueue.hpp"
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include "Task.hpp"

template<class C,class S>
class BlockQueues
{
public:
    BlocckQueue<C> *c_bq;
    BlocckQueue<S> *s_bq;
};

void *productor(void *bqs_)
{
    BlocckQueue<CalTask> *bq = (static_cast<BlockQueues<CalTask, SaveTask> *>(bqs_))->c_bq;
    while (true)
    {
        // 生产活动
        int x = rand() % 1000 + 1; // 在这里先用随机数，构建一个数据
        int y = rand() % 15;
        int operCode = rand() % oper.size();
        CalTask t(x, y, oper[operCode], mymath);
        bq->push(t);
        std::cout << "生产任务： " << t.toTaskString() << std::endl;
        sleep(1);
    }
    return nullptr;
}

void *consumer(void *bqs_)
{
    BlocckQueue<CalTask> *bq = (static_cast<BlockQueues<CalTask, SaveTask> *>(bqs_))->c_bq;
    BlocckQueue<SaveTask> *save_bq = (static_cast<BlockQueues<CalTask, SaveTask> *>(bqs_))->s_bq;

    while(true)
    {
        //消费活动
        CalTask t;
        bq->pop(&t);

        std::string result = t();
        std::cout << "cal thread, 完成计算任务: " << result << " ...done" << std::endl;
        
        // SaveTask save(result, Save);
        // save_bq->push(save);
        // std::cout << "cal thread, 推送存储任务完成..." << std::endl;
        sleep(2);
    }
    return nullptr;
}

void *saver(void *bqs_)
{
    BlocckQueue<CalTask> *bq = (static_cast<BlockQueues<CalTask, SaveTask> *>(bqs_))->c_bq;
    BlocckQueue<SaveTask> *save_bq = (static_cast<BlockQueues<CalTask, SaveTask> *>(bqs_))->s_bq;

    while(true)
    {
        SaveTask t;
        save_bq->pop(&t);
        t();
        std::cout << "save thread保存任务完成..." << std::endl;
    }
    return nullptr;
}

int main()
{
    srand((unsigned long)time(nullptr) ^ getpid());
    BlockQueues<CalTask, SaveTask> bqs;

    bqs.c_bq = new BlocckQueue<CalTask>();
    bqs.s_bq = new BlocckQueue<SaveTask>();

    // pthread_t c, p, s;
    // pthread_create(&p, nullptr, productor, &bqs);
    // pthread_create(&c, nullptr, consumer, &bqs);
    // pthread_create(&s, nullptr, saver, &bqs);

    // pthread_join(c, nullptr);
    // pthread_join(p, nullptr);
    // pthread_join(s, nullptr);

    pthread_t c[2], p[3];
    pthread_create(p, nullptr, productor, &bqs);
    pthread_create(p + 1, nullptr, productor, &bqs);
    pthread_create(p + 2, nullptr, productor, &bqs);
    pthread_create(c, nullptr, consumer, &bqs);
    pthread_create(c + 1, nullptr, consumer, &bqs);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);

    delete bqs.c_bq;
    delete bqs.s_bq;
    return 0;
}