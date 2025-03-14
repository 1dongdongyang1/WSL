#include "ThreadPool.hpp"
#include "Task.hpp"
#include <memory>

int main()
{
    srand((unsigned int)time(nullptr));
    std::unique_ptr<ThreadPool<CalTask>> tp(new ThreadPool<CalTask>());
    tp->run();

    while (1)
    {
        sleep(1);
        int x = rand() % 10 + 1;
        int y = rand() % 5 + 1;
        tp->push(CalTask(x, y, Add));
    }
    return 0;
}