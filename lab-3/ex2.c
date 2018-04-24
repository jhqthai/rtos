#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#define MESSLENGTH 80

char pourIntoPipe[] = "This has been through the pipe!\n";
char collectFromPipe[MESSLENGTH];

int main(void)
{
    int   n, fd[2];
    pid_t pid;

    if ( pipe(fd) < 0 )
      perror("pipe error");
    if ( (pid = fork()) < 0 )
      perror("fork error");
    else if ( pid > 0 )/* parent will do the writing this time */
    {
	  /*write the content of purIntoPipe variable into pipe */
      // your program
	  
	  /*wait child process to finish*/
	  // your program
	  
    }  else {/* child will do the reading. */
	  
	  /*read content from pipe*/
      // your program
	  
	  /*print the content to the monitor*/
      printf("%s", collectFromPipe);
    }
    return (0);
}
