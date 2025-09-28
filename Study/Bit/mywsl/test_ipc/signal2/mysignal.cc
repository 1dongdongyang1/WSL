#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void handler(int signo)
{
    cout << "get signo :" << signo << endl;
}

int main()
{
    struct sigaction act, oact;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);  // 当我们正在处理（递达）某一种信号时，该信号会被屏蔽（阻塞）,
                                // 如果还想屏蔽其他信号，则可以加入到sa_mask里
    sigaction(SIGINT, &act, &oact);

    while(true) sleep(1);
    return 0;
}