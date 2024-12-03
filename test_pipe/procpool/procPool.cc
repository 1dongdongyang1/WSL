#include<iostream>
#include<cstdlib>
#include<cassert>
#include<unistd.h>

#define PROCESS_NUM 5

int main()
{
    //1.建立子进程并建立和子进程通信的管道
    for(int i = 0; i <PROCESS_NUM; i++)
    {
        int fds[2];
        int n = pipe(fds);
        assert(n == 0);
        (void)n;
        pid_t id = fork();
        if(id == 0)
        {
            //子进程，进行处理任务
            close(fds[1]);

            exit(0);
        }
        close(fds[0]);
    }
    //2.走到这里是父进程，要控制子进程

    //3.回收子进程信息
    return 0;
}