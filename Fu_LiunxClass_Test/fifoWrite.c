#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int arge, char argv[])
{
    mkfifo("fifo", 777);
    while (1)
    {
        char buffer[1024];
        scanf("%s", buffer);
        int fd = open("fifo", O_WRONLY);
        write(fd, buffer, sizeof buffer);

        close(fd);
    }
    return 0;
}