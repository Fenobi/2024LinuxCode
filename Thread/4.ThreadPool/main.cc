#include "ThreadPool.hpp"
#include "Task.hpp"
#include <memory>

int main()
{
    // std::unique_ptr<ThreadPool<Task>> tp(new ThreadPool<Task>());
    // tp->run();

    ThreadPool<Task>::getInstance()->run();

    int x, y;
    char op;
    while (1)
    {
        std::cout << "请输入数据1# ";
        std::cin >> x;
        std::cout << "请输入数据2# ";
        std::cin >> y;
        std::cout << "请输入要进行的运算# ";
        std::cin >> op;
        Task t(x, y, op, mymath);
        // tp->push(t);
        ThreadPool<Task>::getInstance()->push(t);
        sleep(1);
    }

    return 0;
}
