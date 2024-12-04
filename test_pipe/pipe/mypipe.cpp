#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// 父进程进行读取，子进程进行写入
int main()
{
    // 第一步，创建管道文件，打开读写端
    int fds[2];
    int n = pipe(fds);
    assert(n == 0);

    // 第二步，fork
    pid_t id = fork();
    assert(id >= 0);
    if (id == 0)
    {
        // 子进程进行写入
        close(fds[0]);
        // 子进程的通信代码
        // string msg = "hello, i am child";
        const char *s = "我是子进程，我正在给你发消息";
        int cnt = 0;
        while (true)
        {
            cnt++;
            char buffer[1024];
            snprintf(buffer, sizeof buffer, "child->parent say:%s[%d][%d]", s, cnt, getpid());
            write(fds[1], buffer, strlen(buffer));
            sleep(1); // 每隔1s写入一次
        }
        // 子进程
        exit(0);
    }

    // 父进程进行读取
    close(fds[1]);
    // 父进程的通信代码
    while (true)
    {
        char buffer[1024];
        ssize_t s = read(fds[0], buffer, sizeof(buffer) - 1);
        if (s > 0)
            buffer[s] = 0;
        cout << "Get Message# " << buffer << " | my pid: " << getpid() << endl;
    }
    n = waitpid(id, nullptr, 0);
    assert(n == id);

    // [0]:读取
    // [1]:写入
    // cout<<"fds[0]:"<<fds[0]<<endl;
    // cout<<"fds[1]:"<<fds[1]<<endl;
    return 0;
}
