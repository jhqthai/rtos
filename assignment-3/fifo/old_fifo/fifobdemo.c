

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

    // Check if file has not been removed
    if(access(FIFONAME, F_OK) != -1){
        printf("File has not been removed.\n");
        if(remove(FIFONAME) < 0) // Remove existing file before writing
            perror("remove");
    }

    /* Try and create the FIFO */
    if(mkfifo(FIFONAME, S_IRWXU) < 0)
    {
        perror("mkfifo");
        exit(1);
    }

    printf("FIFO creation successful\n");

    /* OK - open FIFO and ... */
    if((fifofd = open(FIFONAME, O_WRONLY)) < 0)
    {
        perror("open");
	    exit(1);
    }
    printf("Open success\n");

    /* ... write something into it. */
    write(fifofd, message, strlen(message));
    printf("Write sucess!\n");

    /* close the FIFO */
    close(fifofd);


    exit(0);
}

