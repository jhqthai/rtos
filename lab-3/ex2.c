#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MESSLENGTH 80

char pourIntoPipe[] = "This has been through the pipe!\n";
char collectFromPipe[MESSLENGTH];

int main(void)
{
    int   n, fd[2]; //file descriptor: fd[0] read, fd[1] write
    pid_t pid;

    if ( pipe(fd) < 0 )
      perror("pipe error");
    if ( (pid = fork()) < 0 )
      perror("fork error");
    else if ( pid > 0 )/* parent will do the writing this time */
    {
	    /*write the content of purIntoPipe variable into pipe */
      // your program
      printf("Fd_read: %i\n", fd[0]);
      close(fd[0]); //close file read
      printf("Fd_read: %i\n", fd[0]);
	    /*wait child process to finish*/
	    // your program
	    write(fd[1], pourIntoPipe, strlen(pourIntoPipe));
      printf("Fd_write: %i\n", fd[1]); 
    }  else {/* child will do the reading. */
	  
	  /*read content from pipe*/
      // your program
      n = read(fd[0], collectFromPipe, MESSLENGTH);
	  
	  /*print the content to the monitor*/
      printf("%s", collectFromPipe);
    }
    return (0);
}
