/*! @file
 *  
 *  @brief A program that reads the information from shared memory in prg_1 and outputs the information to user.
 *  
 *  It is recommended to run prg_1 first before running prg_2.
 * 
 *  Compilation instruction: gcc -o prg_2 prg_2.c
 *  Acknowledgement: https://www.youtube.com/watch?v=PRmUybI61cA
 *  
 *  @author John Thai
 *  @date 2018-04-27 
 */ 

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

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

    /* Attach share memory ID to memory */
    shared = shmat(shmid, NULL, 0);
    if (shared == NULL)
        handle_error("Memory attachment failure\n");

    /* Process of writing to memory*/
    p = (double *)shared; // Set shared memory to pointer
    printf("Runtime: %f second(s)\n", *p);
    
    /* Detach from memory */
    retval = shmdt(p);
    if (retval < 0)
        handle_error("Detachment failed");

    exit(EXIT_SUCCESS);
}
