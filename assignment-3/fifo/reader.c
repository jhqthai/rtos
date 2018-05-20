#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFONAME "/tmp/myfifo"
#define MAX_BUF 1024

int main()
{
    int fd;
    char buf[MAX_BUF];

    /* open, read, and display the message from the FIFO */
    fd = open(FIFONAME, O_RDONLY);
    read(fd, buf, MAX_BUF);
    printf("Received: %s\n", buf);
    close(fd);

    return 0;
}