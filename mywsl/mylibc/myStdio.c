#include "myStdio.h"

FILE_* fopen_(const char* pathname, const char* mode) {
    int flags = 0;
    int defaultMode = 0666;

    if (strcmp(mode, "r") == 0) {
        flags |= O_RDONLY;
    } else if (strcmp(mode, "w") == 0) {
        flags |= (O_WRONLY | O_TRUNC | O_CREAT);
    } else if (strcmp(mode, "a") == 0) {
        flags |= (O_WRONLY | O_APPEND | O_CREAT);
    }

    int fd = 0;

    if (flags & O_RDONLY)
        fd = open(pathname, flags);
    else
        fd = open(pathname, flags, defaultMode);
    if (fd < 0) {
        const char* err = strerror(errno);
        write(2, err, strlen(err));
        return NULL;
    }
    FILE_* fp = (FILE_*)malloc(sizeof(FILE_));
    assert(fp);

    fp->flags = SYNC_LINE;      //默认出现\n输出
    fp->fileno = fd;
    fp->cap = SIZE;
    fp->size = 0;
    memset(fp->buffer, 0, SIZE);

    return fp;
}

void fwrite_(const void* ptr, int num, FILE_* fp) {
    // 1.写入到缓存区
    memcpy(fp->buffer + fp->size, ptr, num);
    fp->size += num;
    // 2.判断是否刷新
    if (fp->flags & SYNC_NOW)   fflush_(fp);
    else if (fp->flags & SYNC_FULL) {
        if (fp->size == fp->cap)    fflush_(fp);
    } else if (fp->flags & SYNC_LINE) {
        if (fp->buffer[fp->size - 1] == '\n')   fflush_(fp);  //不考虑"abc\nef"
    }
}

void fflush_(FILE_* fp) {
    if (fp->size > 0) {
        write(fp->fileno, fp->buffer, fp->size);
        fp->size = 0;
    }
}

void fclose_(FILE_* fp) {
    fflush_(fp);
    close(fp->fileno);
}
