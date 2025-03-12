#include "BlockQueue.hpp"
#include "Task.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>

template<class C, class S>
class BlockQueues
{
public:
    BlockQueue<C>* _c_bq;
    BlockQueue<S>* _s_bq;
};

void* consumer(void* args)
{
    BlockQueue<CalTask>* c_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(args))->_c_bq;
    BlockQueue<SaveTask>* s_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(args))->_s_bq;
    while (true)
    {
        CalTask t;
        c_bq->pop(&t);
        std::string result = std::to_string(t());
        std::cout << "推送数据 " << result << std::endl;
        s_bq->push(SaveTask(result, Save));
    }
}

void* producer(void* args)
{
    BlockQueue<CalTask>* c_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(args))->_c_bq;
    while (true)
    {
        int x = rand() % 10 + 1;
        int y = rand() % 5 + 1;
        c_bq->push(CalTask(x,y,Add));
        sleep(1);
    }
}

void* saver(void* args)
{
    BlockQueue<SaveTask>* s_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(args))->_s_bq;
    while (true)
    {
        SaveTask t;
        s_bq->pop(&t);
        t();
        std::cout << "完成保存" << std::endl;
    }
}

int main()
{
    srand((unsigned long)time(0) ^ getpid());

    BlockQueues<CalTask, SaveTask> bqs;
    bqs._c_bq = new BlockQueue<CalTask>();
    bqs._s_bq = new BlockQueue<SaveTask>();

    pthread_t c, p, s;
    pthread_create(&p, nullptr, producer, &bqs);
    pthread_create(&c, nullptr, consumer, &bqs);
    pthread_create(&s, nullptr, saver, &bqs);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    pthread_join(s, nullptr);

    delete bqs._c_bq;
    delete bqs._s_bq;
    return 0;
}