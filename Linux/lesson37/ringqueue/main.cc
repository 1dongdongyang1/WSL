#include "RingQueue.hpp"
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>

#define PNUM 5
#define CNUM 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* producer(void* args)
{
    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);

    while (true)
    {
        sleep(1);
        int data = rand() % 10 + 1;
        rq->push(data);
        pthread_mutex_lock(&mutex);
        std::cout << "生产数据: " << data << std::endl;
        pthread_mutex_unlock(&mutex);
    }
}

void* consumer(void* args)
{
    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);
    while (true)
    {
        int data;
        rq->pop(&data);
        pthread_mutex_lock(&mutex);
        std::cout << "消费数据: " << data << std::endl;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    srand((unsigned int)time(nullptr) ^ getpid() ^ pthread_self());
    RingQueue<int>* rq = new RingQueue<int>();
    pthread_t p[PNUM], c[CNUM];
    for(int i = 0; i < PNUM; i++) pthread_create(p+i, nullptr, producer, rq);
    for(int i = 0; i < CNUM; i++) pthread_create(c+i, nullptr, consumer, rq);
    for(int i = 0; i < PNUM; i++) pthread_join(p[i], nullptr);
    for(int i = 0; i < CNUM; i++) pthread_join(c[i], nullptr);
    return 0;
}