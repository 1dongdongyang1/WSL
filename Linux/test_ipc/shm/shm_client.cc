#include "comm.hpp"

int main()
{
    //创建共享内存
    key_t k = getKey();
    printf("key : 0x%x\n",k);
    int shmid = getShm(k);
    printf("shmid : %d\n",shmid);
    //关联进程地址空间和共享内存
    char* start = (char*)attachShm(shmid);
    printf("attach sucess, address start: %p\n",start);

    //使用
    const char* message = "hello server, 我是另一个进程, 正在和你通信";
    pid_t id = getpid();
    int cnt = 1;
    while (true)
    {
        snprintf(start,MAX_SIZE,"%s[pid:%d][消息编号:%d]",message,id,cnt++);
        sleep(1);
    }
    

    //去关联进程地址空间和共享内存
    detachShm(start);
    return 0;
}