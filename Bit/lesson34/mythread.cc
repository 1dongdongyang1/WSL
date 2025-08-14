#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include "Mutex.hpp"

#define NUM 4

int tickets = 10000;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

class ThreadData
{
public:
    ThreadData(const std::string& name, pthread_mutex_t* lock)
        :_name(name), _lock(lock)
    {
    }
public:
    std::string _name;
    pthread_mutex_t* _lock;
};

void* getticket(void* args)
{
    // const char* name = static_cast<const char*>(args);
    ThreadData* td = static_cast<ThreadData*>(args);
    while (1)
    {
        {
            LockGuard lockguard(&lock);
            // pthread_mutex_lock(td->_lock);
            if (tickets > 0)
            {
                usleep(1000);
                std::cout << td->_name << " get getticket, tickets: " << tickets-- << std::endl;
                // usleep(1000);
                // pthread_mutex_unlock(td->_lock);
            }
            else
            {
                // pthread_mutex_unlock(td->_lock);
                break;
            }
        }
        usleep(1000);
    }

    return nullptr;

}

int main()
{
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, nullptr);

    std::vector<pthread_t> tids(NUM);
    for (int i = 0; i < NUM; i++)
    {
        char namebuffer[64];
        snprintf(namebuffer, sizeof namebuffer, "thread %d", i + 1);
        ThreadData* td = new ThreadData(namebuffer, &lock);
        pthread_create(&tids[i], nullptr, getticket, (void*)td);
    }

    for (const auto& tid : tids)
    {
        pthread_join(tid, nullptr);
    }


    pthread_mutex_destroy(&lock);

    // pthread_t tid1,tid2,tid3,tid4;

    // pthread_create(&tid1, nullptr, getticket, (void*)"thread 1");
    // pthread_create(&tid2, nullptr, getticket, (void*)"thread 2");
    // pthread_create(&tid3, nullptr, getticket, (void*)"thread 3");
    // pthread_create(&tid4, nullptr, getticket, (void*)"thread 4");

    // pthread_join(tid1, nullptr);
    // pthread_join(tid2, nullptr);
    // pthread_join(tid3, nullptr);
    // pthread_join(tid4, nullptr);

    return 0;
}