#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM 1024
#define OPT_NUM 64

#define NONE_REDIR 0
#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3
#define trimSpace(start)                 \
    do {                                 \
        while (isspace(*start)) ++start; \
    } while (0)

char lineCommand[NUM];
char *myargv[OPT_NUM];
int lastCode = 0;
int lastSig = 0;

int redirType = NONE_REDIR;
char *redirFile = NULL;

//"ls -a -l -h > log.txt" -> "ls -a -l -h" "log.txt"
void commandCheck(char *commands) {
    assert(commands);
    char *start = commands;
    char *end = commands + strlen(commands);

    while (start < end) {
        if (*start == '>') {
            *start++ = '\0';
            if (*start == '>') {
                redirType = APPEND_REDIR;
                start++;
            } else {
                redirType = OUTPUT_REDIR;
            }
            trimSpace(start);
            redirFile = start;
            break;
        } else if (*start == '<') {
            //"cat < log.txt"
            *start++ = '\0';
            trimSpace(start);
            //填写重定向信息
            redirType = INPUT_REDIR;
            redirFile = start;
            break;
        } else {
            start++;
        }
    }
}

int main() {
    while (1) {
        //初始化
        redirType = NONE_REDIR;
        redirFile = NULL;
        //输入提示符
        printf("用户名@主机名 当前路径# ");
        fflush(stdout);

        //获取用户输入，输入的时候，输入\n
        char *s = fgets(lineCommand, sizeof(lineCommand) - 1, stdin);
        assert(s != NULL);
        (void)s;
        //清除最后一个\n，abcd\n
        lineCommand[strlen(lineCommand) - 1] = 0;
        // printf("test: %s\n", lineCommand);

        //"ls -a -l -h > log.txt" -> "ls -a -l -h" "log.txt"
        commandCheck(lineCommand);
        //字符串切割
        //"ls -a -l -h" -> "ls" "-a" "-l" "-h" -> 1 -> n
        myargv[0] = strtok(lineCommand, " ");
        //如果没有字串了，strtok -> NULL, myargv[end] = NULL
        int i = 1;
        while (myargv[i++] = strtok(NULL, " "))
            ;

        if (myargv[0] != NULL && strcmp(myargv[0], "cd") == 0) {
            if (myargv[1] != NULL) chdir(myargv[1]);
            continue;
        }

        if (myargv[0] != NULL && strcmp(myargv[0], "echo") == 0) {
            if (strcmp(myargv[1], "$?") == 0) {
                printf("%d, %d\n", lastCode, lastSig);
            } else {
                printf("%s\n", myargv[1]);
            }
            continue;
        }

        //测试是否成功,条件编译
#ifdef DEBUG
        for (int i = 0; myargv[i]; i++) {
            printf("myargv[%d]: %s\n", i, myargv[i]);
        }
#endif
        //执行命令
        pid_t id = fork();
        assert(id != -1);

        if (id == 0) {
            //因为命令是子进程执行的，真正重定向的工作是子进程来完成的
            //如何重定向，父进程是要给子进程提供信息的

            int fd;
            switch (redirType) {
                case NONE_REDIR:
                    break;
                case INPUT_REDIR:
                    fd = open(redirFile, O_RDONLY);
                    dup2(fd, 0);
                    break;
                case OUTPUT_REDIR:
                case APPEND_REDIR:
                    int flags = O_CREAT | O_WRONLY;
                    if (redirType == OUTPUT_REDIR)
                        flags |= O_TRUNC;
                    else
                        flags |= O_APPEND;
                    fd = open(redirFile, flags, 0666);
                    dup2(fd, 1);
                    break;
                default:
                    printf("bug");
                    break;
            }

            execvp(myargv[0], myargv);
            exit(1);
        }

        int status = 0;
        waitpid(id, &status, 0);
        lastCode = ((status >> 8) & 0xFF);
        lastSig = (status & 0x7F);
    }
}
