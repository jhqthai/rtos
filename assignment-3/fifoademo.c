

/* This program creates a FIFO in /tmp and
   waits for for another program
   to put something into the FIFO. It will
   then print out whatever it finds in the
   FIFO.
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "fifodemo.h"

int main(void)
{
    int fifofd;  /* FIFO file descriptor */
    int n;       /* Number of chars read */
    char datafromFIFO[MESSLENGTH];

    /* Try and create the FIFO */
    if(mkfifo(FIFONAME, S_IRUSR | S_IWUSR) < 0)
    {
        perror("mkfifo");
        exit(1);
    }

    printf("FIFO creation successful - now waiting for a writer ...\n");

    /* OK - open FIFO and read what comes through */
    if((fifofd = open(FIFONAME, O_RDONLY)) < 0)
    {
        perror("open");
	exit(2);
    }
    
    n = read(fifofd, datafromFIFO, MESSLENGTH);

    if(n == 0)
        printf("nobody put anything into the FIFO\n");
    else
    {
        datafromFIFO[n] = '\0'; /* Put a null char at the end */
        printf("got the message: %s\n", datafromFIFO);
    }

    /* Close the FIFO and destroy it */
    close(fifofd);
    if(unlink(FIFONAME) < 0)
    {
	perror("unlink");
	exit(3);
    }

    exit(0);
}

