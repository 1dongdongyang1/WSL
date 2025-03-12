#include "Thread.hpp"
#include <memory>
#include <string>
#include <unistd.h>

using namespace std;

void* thread_run(void* args)
{
    string s = static_cast<const char*>(args);
    while(1)
    {
        std::cout<< "thread :" << s << endl;
        sleep(1);
    }
}

int main()
{
    std::unique_ptr<Thread> thread1(new Thread(thread_run, (void*)"hello thread", 0));
    thread1->start();

    thread1->join();
 
    return 0;
}