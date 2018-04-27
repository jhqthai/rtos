/*************
 * gcc -o shmread shmread.c
 * 
 * 
 * 
 * ***********/


/* Include necessary libraries */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>


int main()
{
    // Declare variables
    int retval, shmid;
    void *memory = NULL;
    char *shared;

    // Initialised share memory
    shmid = shmget((key_t)123456, 6, IPC_CREAT|0666);
    if (shmid < 0){
        perror("Key creatino failed");
        shmid = shmget((key_t)123456, 6, 0666);
    }
    printf("Key generated: %d\n", shmid);

    // Attach memory id
    memory = shmat(shmid, NULL, 0);
    if (memory == NULL)
    {
        perror("Memory attachment failure\n");
        return -1;
    }

    // read off memory
    shared = (char*)memory; // Set memory to pointer
    printf("Message: %s\n", shared);

    // Detach from memory
    retval = shmdt(shared);
    if (retval < 0){
        perror("Detachment failed");
        return -1;
    }
}