#include "comm.hpp"

int main()
{
    int wfd = open(NAMED_PIPE,O_WRONLY);
    if(wfd < 0) exit(0);

    //write
    char buffer[1024];
    while (true)
    {
        std::cout << "Please Say# ";
        fgets(buffer,sizeof(buffer),stdin);
        if(strlen(buffer) > 0) buffer[strlen(buffer)-1] = 0;
        ssize_t n = write(wfd,buffer,strlen(buffer));
        assert(n == strlen(buffer));
        (void)n;
    }
    
    close(wfd);
    return 0;
}