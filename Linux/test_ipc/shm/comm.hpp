#ifndef _COMM_HPP_
#define _COMM_HPP_

#include<iostream>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<cerrno>
#include<unistd.h>
#include<cstring>
#include<cstdlib>
#include<stdio.h>


#define PATHNAME "."
#define PROJ_ID 0x66
#define MAX_SIZE 4096

key_t getKey()
{
    key_t k = ftok(PATHNAME,PROJ_ID);
    if(k == -1)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(1);
    }
    return k;
}

int shmGetHelper(key_t k, int flags)
{
    int shmid = shmget(k,MAX_SIZE,flags);
    if(shmid == -1)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(2);
    }
    return shmid;
}

int createShm(key_t k)
{
    return shmGetHelper(k, IPC_CREAT | IPC_EXCL | 0600);
}


int getShm(key_t k)
{
    return shmGetHelper(k, IPC_CREAT);
}

void* attachShm(int shmid)
{
    void* mem = shmat(shmid,nullptr,0);
    if((long long)mem == -1L)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(3);
    }
    return mem;
}

void detachShm(void* start)
{
    if(shmdt(start) == -1)
        std::cerr << errno << ":" << strerror(errno) << std::endl;
}

void delShm(int shmid)
{
    if(shmctl(shmid,IPC_RMID,nullptr) == -1)
        std::cerr << errno << ":" << strerror(errno) << std::endl;
}

#endif