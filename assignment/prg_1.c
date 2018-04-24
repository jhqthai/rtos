/******
 * Instruction:
 * 
 * 
 * *****/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

/* Global variables delcaration */
pthread_mutex_t mutex; // Mutex lock
sem_t semA, semB, semC; // Semaphores
pthread_t tidA, tidB, tidC; // Thread ID
pthread_attr_t attr; // Set of thread attributes

void *runnerA(void *param); // Tread A function
void *runnerB(void *param); // Thread B function
void *runnerC(void *param); // Thread C funtion
void initData();

int main(int argc, char *argv[])
{
    /* TBU: Probably error checking */
    // if(argc!=2){
	//     fprintf(stderr,"usage: a.out <integer value>\n");
	//     return -1;
    // }
    // if(atoi(argv[1])<0){
	//     fprintf(stderr,"%d must be >=0\n",atoi(argv[1]));
	//     return -1;
    // }

    initData(); //Initialise threads and semaphores

    //pthread_attr_init(&attr); //TODO: Might be uneccessary

    // TODO: Get data from data.txt

    // TODO: Maybe the last param is what should be passed in?
    /* Create threads */
    pthread_create(&tidA, &attr, runnerA, param); // Create thread A
    pthread_create(&tidB, &attr, runnerB, param); // Create thread B
    pthread_create(&tidC, &attr, runnerC, param); // Create thread C

    sem_post(&semA); // Unlock semaphore A --This will allow thread A to run first

    /* Wait for thread to exit */
    pthread_join(tidA, NULL);
    pthread_join(tidB, NULL);
    pthread_join(tidC, NULL);

    // TODO: Probably do something with the timer here
}


void initData()
{
    pthread_mutex_init(&mutex, NULL); // Create mutex lock

    sem_init(&semA, 0, 0); // Create semaphore A and block subroutine
    sem_init(&semB, 0, 0); // Create semaphore B and block subroutine
    sem_init(&semC, 0, 0); // Create semaphore C and block subroutine

    pthread_attr_init(&attr); // Initialise default attributes
}


