#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM 10

typedef void (*func_t)();  //函数指针

func_t handlerTask[NUM];

void task1() { printf("handler task1\n"); }
void task2() { printf("handler task2\n"); }
void task3() { printf("handler task3\n"); }

void loadTask() {
    memset(handlerTask, 0, sizeof(handlerTask));
    handlerTask[0] = task1;
    handlerTask[1] = task2;
    handlerTask[2] = task3;
}

int main() {
    pid_t id = fork();
    assert(id != -1);
    if (id == 0) {
        // child
        int cnt = 5;
        while (cnt) {
            printf("child running, pid:%d, ppid:%d, cnt:%d\n", getpid(),
                   getppid(), cnt--);
            sleep(1);
        }
        exit(10);
    }

    // parent
    loadTask();
    int status = 0;
    while (1) {
        int ret = waitpid(id, &status, WNOHANG);  // WNOHANG:非阻塞
        if (ret == 0) {
            printf(
                "wait done, but child is running..., parent running other "
                "things\n");
            for (int i = 0; handlerTask[i] != NULL; i++) {
                handlerTask[i]();
            }
        } else if (ret > 0) {
            printf("wait success, exit code:%d, sig:%d\n", (status >> 8) & 0xFF,
                   status & 0x7F);
            break;
        } else {
            printf("waitpid call failed\n");
            break;
        }
        sleep(1);
    }

    //    //parent
    //    int status = 0;
    //    int ret = waitpid(id,&status,0);
    //    if(ret>0)
    //    {
    //        printf("wait success, exit code:%d, sig:%d\n", (status>>8)&0xFF,
    //        status & 0x7F);
    //    }

    return 0;
}
