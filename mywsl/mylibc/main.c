#include "myStdio.h"

int main()
{
    FILE_* fp = fopen_("./log.txt","w");
    if(!fp) return 1;

    const char* msg = "hello ddy\n";
    fwrite_(msg,strlen(msg),fp);
    fclose_(fp);
    return 0;
}
