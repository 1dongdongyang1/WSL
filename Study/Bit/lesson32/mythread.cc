#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <cassert>

#define NUM 10

using namespace std;

struct ThreadData
{
    pthread_t tid;
    char namebuffer[64];
};

void* start_routine(void* args)
{
    // sleep(1);
    ThreadData* td = static_cast<ThreadData*>(args); //安全的进行强制类型转化
    int cnt = 10;
    while(cnt)
    {
        cout << "new thread create success, name: " << td->namebuffer <<" cnt: " << cnt-- <<endl;
        sleep(1);
    }
    // delete td;
    pthread_exit((void*)100);
}
int main()
{
    // 1.创建一批线程
    vector<ThreadData*> tds;
    for(int i = 0; i < NUM; i++)
    {
        ThreadData* td = new ThreadData();
        snprintf(td->namebuffer, sizeof(td->namebuffer), "%s : %d", "thread", i+1);
        pthread_create(&td->tid, nullptr, start_routine, td);
        tds.push_back(td);
    }

    for(auto& iter : tds)
    {
        cout<<"create thread: " << iter->namebuffer << " : " << iter->tid << " success" << endl;
    }

    for(auto& iter : tds)
    {
        void* ret = nullptr;
        int n = pthread_join(iter->tid, &ret);
        assert(n == 0);
        (void)n;
        cout << "join :" << iter->namebuffer << " : " << (long long)ret << endl;
        delete iter;
    }
    // pthread_t id;
    // pthread_create(&id, nullptr, start_routine, (void*)"new thread");
    // while(true)
    // {
    //     cout << "new thread create success, name: main thread" <<endl;
    //     sleep(1);
    // }
    return 0;
}
    