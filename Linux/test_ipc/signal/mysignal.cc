#include <unistd.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <vector>

// void Usage(const std::string& proc)
// {
//     std::cout << "\nUsage:" << proc << " pid signo\n" << std::endl;
// }

// int main(int argc, char* argv[])
// {
//     if(argc != 3)
//     {
//         Usage(argv[0]);
//         exit(1);
//     }
//     pid_t pid = atoi(argv[1]);
//     int signo = atoi(argv[2]);
//     int n = kill(pid,signo);
//     if(n != 0) perror("kill");
// }

// #define BLOCK_SIGNAL 2
#define MAX_SIGNUM 31

static std::vector<int> sigarr = {2};

static void show_pending(const sigset_t& pending)
{
    for(int signo = MAX_SIGNUM; signo >= 1; signo--)
    {
        if(sigismember(&pending, signo)) std::cout<<"1";
        else std::cout<<"0";
    }
    std::cout<<std::endl;
}

static void myhandler(int signo)
{
    std::cout << signo << "号信号递达"<<std::endl;
}

int main()
{
    for(const auto& sig : sigarr) signal(sig, myhandler);
    // 1.先尝试屏蔽指定信号
    sigset_t block,oblock,pending;
    // 1.1初始化
    sigemptyset(&block);
    sigemptyset(&oblock);
    sigemptyset(&pending);
    // 1.2添加要屏蔽的信号
    // sigaddset(&block,BLOCK_SIGNAL);
    for(const auto& sig : sigarr) sigaddset(&block, sig);
    // 1.3开始屏蔽，设置进内核(进程)
    sigprocmask(SIG_SETMASK, &block, &oblock);

    // 2.遍历打印pending信号集
    int cnt = 5;
    while(true)
    {
        // 2.1初始化
        sigemptyset(&pending);
        // 2.2获取
        sigpending(&pending);
        // 2.3打印
        show_pending(pending);
        // 2.4慢一点
        sleep(1);
        if(cnt-- == 0)
        {
            sigprocmask(SIG_SETMASK, &oblock, &block);
            std::cout<< "恢复对信号的屏蔽，不屏蔽任何信号";
        }
    }
    return 0;
}