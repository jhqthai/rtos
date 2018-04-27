/*************
 * gcc -o shmwrite shmwrite.c
 * 
 * 
 * 
 * ***********/


#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int retval, shmid;
    void *memory = NULL;
    char *shared;

    /* Initialise share memory*/
    shmid = shmget((key_t)123456, 6, IPC_CREAT|0666);
    if (shmid < 0){
        perror("Key creatino failed");
        shmid = shmget((key_t)123456, 6, 0666);
    }
    printf("Key generated: %d\n", shmid);

    /* Attach share memory ID to memory */
    memory = shmat(shmid, NULL, 0);
    if (memory == NULL)
    {
        perror("Memory attachment failure\n");
        return -1;
    }

    /* Process of writing to memory*/
    shared = (char *)memory; // Set memory to pointer
    memset(shared, '\0', 6); // Clearing 6 bits of data before transfering
    memcpy(shared, "hello", 6); // Writing to memory!
    

    /* Detach from memory */
    retval = shmdt(shared);
    if (retval < 0){
        perror("Detachment failed");
        return -1;
    }
    return 0;
}