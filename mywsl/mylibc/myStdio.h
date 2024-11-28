#pragma once

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE 1024
#define SYNC_NOW 1
#define SYNC_LINE 2
#define SYNC_FULL 4

typedef struct FILE_ {
  int flags;
  int fileno;
  int cap;
  int size;
  char buffer[SIZE];
} FILE_;

FILE_ *fopen_(const char *pathname, const char *mode);
void fwrite_(const void *ptr, int num, FILE_ *fp);
void fclose_(FILE_ *fp);
void fflush_(FILE_ *fp);
