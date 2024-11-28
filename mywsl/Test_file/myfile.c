#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FILE_NAME "log.txt"

/*#define ONE (1<<0)*/
/*#define TWO (1<<1)*/
/*#define THREE (1<<2)*/
/*#define FOUR (1<<4)*/
/**/
/*void show(int flags) {*/
/*    if(flags & ONE) printf("one\n");*/
/*    if(flags & TWO) printf("two\n");*/
/*    if(flags & THREE) printf("three\n");*/
/*    if(flags & FOUR) printf("four\n");*/
/*}*/

int main() {
    /*int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);*/
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char buffer[1024];
    ssize_t num = read(fd, buffer, sizeof(buffer) - 1);
    if (num > 0) buffer[num] = 0;
    printf("%s", buffer);

    /*int cnt = 5;*/
    /**/
    /*char outBuffer[64];*/
    /*while (cnt) {*/
    /*    sprintf(outBuffer, "%s:%d\n", "hello", cnt--);*/
    /*    write(fd, outBuffer, strlen(outBuffer));*/
    /*}*/
    /**/
    close(fd);
    /*FILE* fp = fopen(FILE_NAME, "r");*/
    /*if (fp == NULL) {*/
    /*    perror("fopen");*/
    /*    return 1;*/
    /*}*/

    /*char buffer[64];*/
    /*while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {*/
    /*    buffer[strlen(buffer) - 1] = 0;*/
    /*    puts(buffer);*/
    /*}*/

    /*fclose(fp);*/
}
