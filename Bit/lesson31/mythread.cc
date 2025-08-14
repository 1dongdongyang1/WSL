#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <cassert>
#include <unistd.h>


using namespace std;

//新线程
void* thread_routine(void* args)
{
    while(true)
    {
        cout << "我是新线程" << endl;
        sleep(1);
    }

}

int main()
{
    pthread_t tid;
    int n = pthread_create(&tid, nullptr, thread_routine, (void*)"thread one");
    assert(0 == n);
    (void)n;

    //主线程
    while(true)
    {
        char tidbuffer[64];
        snprintf(tidbuffer, sizeof(tidbuffer), "0x%x", tid);
        cout << "我是主线程, 创建的线程的tid: " << tidbuffer << endl;
        sleep(1);
    }

    return 0;
}