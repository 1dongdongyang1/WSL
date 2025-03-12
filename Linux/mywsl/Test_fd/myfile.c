#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    umask(0);
    /*int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);*/
    int fd = open("log.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    dup2(fd, 0);
    char line[64];

    while (1) {
        printf("> ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        printf("%s", line);
    }

    /*dup2(fd, 1);*/
    /*printf("open fd:%d\n", fd);*/
    /**/
    close(fd);
    return 0;
}
