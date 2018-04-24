/******
 * Instruction:
 * 
 * 
 * *****/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

/* Constant definition */
#define DATA_FILE "data.txt"
#define SRC_FILE "src.txt"


/* Global variables delcaration */
pthread_mutex_t mutex; // Mutex lock
sem_t semA, semB, semC; // Semaphores
pthread_t tidA, tidB, tidC; // Thread ID
pthread_attr_t attr; // Set of thread attributes
//TODO: Probably have to set up pipe stuff here?

//TODO: something with struct and datafile

void *runnerA(void *param); // Tread A function
void *runnerB(void *param); // Thread B function
void *runnerC(void *param); // Thread C funtion
void initData();

int main(int argc, char *argv[])
{
    /* Initialise pipe */
    int fd[2];
    // Error handling
    if (pipe(fd)<0)
        perror("Pipe error");

    /* Opening file */
    FILE *data_fp = fopen(DATA_FILE, "r"); // "data.txt" file read
    // Error handling
    if (data_fp == NULL){
        perror("Data file error");
        // Pipe status error handling
        if (fd[0]) {
            if (close(fd[0])) 
                perror("File descriptor error");
        }
        if (fd[1]) {
            if (close(fd[1]))
                perror("File descriptor error");
        }
        return 1;
    }

    FILE *src_fp = fopen(SRC_FILE, "w"); // "src.txt" file write
    //Error handling
    if (src_fp == NULL){
        perror("Src file error");
        if (fclose(data_fp))
            perror("Data file/close error");
        return 1;
    }




    initData(); // Initialise threads and semaphores

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

    return 0; // Program excuted all good!
}

void initData()
{
    pthread_mutex_init(&mutex, NULL); // Create mutex lock

    sem_init(&semA, 0, 0); // Create semaphore A and block subroutine
    sem_init(&semB, 0, 0); // Create semaphore B and block subroutine
    sem_init(&semC, 0, 0); // Create semaphore C and block subroutine

    pthread_attr_init(&attr); // Initialise default attributes
}

void *runnerA(void *param)
{
    sem_wait(&semA); // Wait till unlocked
    pthread_mutex_lock(&mutex); // Lock mutex to prevent concurrent threads execution

    //TODO: Write data to pipe

    //TEST
    printf("Thread A\n");

    pthread_mutex_unlock(&mutex); // Release mutex lock
    sem_post(&semB); // Release and unlock semaphore B 

}

void *runnerB(void *param)
{
    sem_wait(&semB); // Wait till unlocked
    pthread_mutex_lock(&mutex); // Lock mutex to prevent concurrent threads execution

    //TODO: reads data from pipe

    //TODO: pass data to thread C

    //TEST
    printf("Thread B\n");

    pthread_mutex_unlock(&mutex); // Release mutex lock
    sem_post(&semC); // Release and unlock semaphore C 

}

void *runnerC(void *param)
{
    sem_wait(&semC); // Wait till unlocked
    pthread_mutex_lock(&mutex); // Lock mutex to prevent concurrent threads execution

    //TODO: read passed data

    //TODO: Dtermine data region

    //TODO: write data to src.txt

    //TEST
    printf("Thread C\n");

    pthread_mutex_unlock(&mutex); // Release mutex lock
    sem_post(&semC); // Release and unlock semaphore C 

}
