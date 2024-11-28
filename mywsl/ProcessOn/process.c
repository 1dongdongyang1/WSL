#include"process.h"

char *style[S_NUM] = {"-",".","#",">","+"};

void ProcessOn()
{
    int cnt = 0;
    char bar[NUM];
    memset(bar,'\0',sizeof(bar));

    const char *lable = "|\\-/";

    while(cnt<=100)
    {
        printf("[%-100s][%d%%][%c]\r",bar,cnt,lable[cnt%4]);
        fflush(stdout);
        bar[cnt++] = style[N][0];
        usleep(50000);
    }
        printf("\n");
}
