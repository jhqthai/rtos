#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFONAME "/tmp/myfifo"

int main()
{
    int fd;

    /* create the FIFO (named pipe) */
    mkfifo(FIFONAME, 0666);

    /* write "Hi" to the FIFO */
    fd = open(FIFONAME, O_WRONLY);
    write(fd, "Hi", sizeof("Hi"));
    close(fd);

    /* remove the FIFO */
    unlink(FIFONAME);

    return 0;
}