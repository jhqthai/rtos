#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main()
{
    int retval, shmid;
    void *shared = NULL; // Share memory
    double *p;

    /* Initialise share memory*/
    shmid = shmget((key_t)123456, 6, IPC_CREAT|0666);
    if (shmid < 0){
        perror("Key creation failed");
        shmid = shmget((key_t)123456, 6, 0666);
    }
    printf("Key generated: %d\n", shmid);

    /* Attach share memory ID to memory */
    shared = shmat(shmid, NULL, 0);
    if (shared == NULL)
    {
        perror("Memory attachment failure\n");
        exit(EXIT_FAILURE);
    }

    /* Process of writing to memory*/
    p = (double *)shared; // Set shared memory to pointer
    printf("Runtime: %f second(s)\n", *p);
    
    /* Detach from memory */
    retval = shmdt(p);
    if (retval < 0){
        perror("Detachment failed");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
