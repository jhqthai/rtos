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

int main(itn argc, char *argv[])
{

}

void initData()
{
    pthread_mutex_init(&mutex, NULL); // Create mutex lock

    sem_init(&semA, 0, 0); // Create semaphore A and block subroutine
    sem_init(&semB, 0, 0); // Create semaphore B and block subroutine
    sem_init(&semC, 0, 0); // Create semaphore C and block subroutine

    pthread_attr_init(&attr); // Initialise default attributes
}


