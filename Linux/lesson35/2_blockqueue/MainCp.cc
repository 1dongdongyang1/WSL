#include "BlockQueue.hpp"
#include "Task.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>

int Add(int x, int y)
{
    return x + y;
}

void* consumer(void* args)
{
    BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(args);
    while (true)
    {
        Task t;
        bq->pop(&t);
        std::cout << t() << std::endl;
        // int data;
        // bq->pop(&data);
        // std::cout << "消费数据: " << data << std::endl;
        // sleep(1);
    }
}

void* producer(void* args)
{
    BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(args);
    while (true)
    {
        int x = rand() % 10 + 1;
        int y = rand() % 5 + 1;
        bq->push(Task(x,y,Add));
        // int data = rand() % 10 + 1;
        // bq->push(data);
        // std::cout << "生产数据: " << data << std::endl;
        // sleep(1);
    }
}

int main()
{
    srand((unsigned long)time(0) ^ getpid());
    BlockQueue<Task>* bq = new BlockQueue<Task>();
    pthread_t c, p;
    pthread_create(&c, nullptr, consumer, bq);
    pthread_create(&p, nullptr, producer, bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    return 0;
}