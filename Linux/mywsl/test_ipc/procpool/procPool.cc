#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctime>
#include <cassert>
#include <unistd.h>
#include <vector>
#include <string>

#define PROCESS_NUM 5

#define MakeSeed() srand((unsigned int)time(nullptr))

//////////////////////////////////////////////模拟子进程需要完成的任务///////////////////////////
typedef void (*func_t)(); // 函数指针类型

void downloadTask()
{
    std::cout << getpid() << ": 下载任务" << std::endl;
    sleep(1);
}

void ioTask()
{
    std::cout << getpid() << ": IO任务" << std::endl;
    sleep(1);
}

void flushTask()
{
    std::cout << getpid() << ": 刷新任务" << std::endl;
    sleep(1);
}

void loadTaskFunc(std::vector<func_t> &funcMap)
{
    funcMap.push_back(downloadTask);
    funcMap.push_back(ioTask);
    funcMap.push_back(flushTask);
}
//////////////////////////////////////////////多进程程序////////////////////////////////////////
class subEp // Endpoint
{
public:
    subEp(pid_t subId, int writeFd)
        : _subId(subId), _writeFd(writeFd)
    {
        char nameBuffer[1024];
        snprintf(nameBuffer, sizeof nameBuffer, "process-%d[pid(%d)-fd(%d)]", num++, _subId, _writeFd);
        _name = nameBuffer;
    }

public:
    static int num;
    std::string _name;
    pid_t _subId;
    int _writeFd;
};

int subEp::num = 0;

int recvTask(int readFd)
{
    int code = 0;
    ssize_t s = read(readFd, &code, sizeof code);
    if(s == 4) return code;
    else return -1;
} 

void sendTask(const subEp &process, int taskNum)
{
    std::cout << "send task num: " << taskNum << " send to -> " << process._name << std::endl;
    int n = write(process._writeFd, &taskNum, sizeof taskNum);
    assert(n == sizeof(int));
    (void)n;
}

void createSubProcess(std::vector<subEp> &subs, std::vector<func_t> &funcMap)
{
    std::vector<int> deleteFd;
    for (int i = 0; i < PROCESS_NUM; i++)
    {
        int fds[2];
        int n = pipe(fds);
        assert(n == 0);
        (void)n;
        pid_t id = fork();
        if (id == 0)
        {
            // 子进程，进行处理任务
            for(int i = 0; i<deleteFd.size();i++) close(deleteFd[i]);
            close(fds[1]);
            while (true)
            {
                // 1.获取命令码，如果没有发送，子进程应该堵塞
                int commandCode = recvTask(fds[0]); 
                // 2.完成任务
                if(commandCode >= 0 && commandCode < funcMap.size()) funcMap[commandCode]();
                else if(commandCode == -1) break;
            }
            exit(0);
        }
        close(fds[0]);
        subs.push_back(subEp(id, fds[1])); // 匿名对象，直接调用push_back的右值引用版本
        deleteFd.push_back(fds[1]);
    }
}

void loadBlanceContrl(const std::vector<subEp>& subs, const std::vector<func_t>& funcMap, int count)
{
    int processnum = subs.size();
    int tasknum = funcMap.size();
    bool forever = count == 0 ? true : false;
    while (true)
    {
        // 1.选择一个子进程
        int subIdx = rand() % processnum;
        // 2.选择一个任务
        int taskIdx = rand() % tasknum;
        // 3.任务发送给选择的进程
        sendTask(subs[subIdx], taskIdx);
        sleep(1);
        if(!forever)
        {
            count--;
            if(count == 0) break;
        }
    }

    for(int i = 0; i<subs.size(); i++) close(subs[i]._writeFd);
}

void waitProcess(std::vector<subEp>& processes) 
{
   int processnum = processes.size();
   for(int i = 0; i<processnum; i++)
   {
       waitpid(processes[i]._subId,nullptr,0); 
       std::cout<<"wait sub process success ...:"<< processes[i]._subId <<std::endl;
   } 
}

int main()
{
    MakeSeed();
    // 1.建立子进程并建立和子进程通信的管道
    // 1.1 加载方法表
    std::vector<func_t> funcMap;
    loadTaskFunc(funcMap);
    // 1.2 创建子进程，并且维护好父子通道
    std::vector<subEp> subs;
    createSubProcess(subs, funcMap);

    // 2.走到这里是父进程，控制子进程, 负载均衡的向子进程发送命令码
    int taskCnt = 3; // 0:永远进行
    loadBlanceContrl(subs, funcMap, taskCnt);

    // 3.回收子进程信息
    waitProcess(subs); 
    return 0;
}