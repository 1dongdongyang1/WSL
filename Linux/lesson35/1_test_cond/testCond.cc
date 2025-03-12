#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>

int tickes = 1000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* start_routine(void* args)
{
    std::string name = static_cast<const char*>(args);
    while (true)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        std::cout << name << " -> " << tickes-- << std::endl;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    std::vector<pthread_t> ts(5);
    for (int i = 0; i < 5; i++)
    {
        char* name = new char[64];
        snprintf(name, 64 , "thread %d", i + 1);
        pthread_create(&ts[i], nullptr, start_routine, (void*)name);
    }

    while (true)
    {
        sleep(1);
        std::cout << " main thread wake up ont thread ..." << std::endl;
        // pthread_cond_signal(&cond);
        pthread_cond_broadcast(&cond);
    }

    for (auto t : ts)
    {
        pthread_join(t, nullptr);
    }
    return 0;
}