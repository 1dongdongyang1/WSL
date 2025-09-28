#include "comm.hpp"

int main()
{
    //创建共享内存
    key_t k = getKey();
    printf("0x%x\n",k);
    int shmid = createShm(k);
    printf("shmid : %d\n",shmid);

    //关联进程地址空间和共享内存
    char* start = (char*)attachShm(shmid);
    printf("attach sucess, address start: %p\n",start);

    //使用
    while (true)
    {
        printf("client say: %s\n", start);
        sleep(1);
    }
    

    //去关联进程地址空间和共享内存
    detachShm(start);

    //删除共享内存
    delShm(shmid);
    return 0;
}