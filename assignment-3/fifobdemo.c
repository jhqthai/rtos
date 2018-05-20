

/* 
 * This program opens a FIFO in /tmp and writes something into it.
 * The "fifoademo" program needs to have been started first, so that
 * the FIFO has been previously created.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "fifodemo.h"

int main(void)
{
    char message[] = "Hi - this is a message!\n";
    int fifofd;  /* FIFO file descriptor */

    /* OK - open FIFO and ... */
    if((fifofd = open(FIFONAME, O_WRONLY)) < 0)
    {
        perror("open");
	exit(1);
    }
    
    /* ... write something into it. */
    write(fifofd, message, strlen(message));

    /* close the FIFO */
    close(fifofd);

    exit(0);
}

