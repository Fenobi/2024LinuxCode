#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int arge,char argv[])
{
    mkfifo("fifo", 777);
    while(1)
    {
        int fd = open("fifo", O_RDONLY);
        char buffer[1024];
        read(fd, buffer, sizeof buffer);
        printf("%s\n", buffer);
        
        close(fd);
    }
    return 0;
}