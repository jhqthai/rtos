/*! @file
 *  
 *  @brief A multi-threads program for read and writing data using pipe-line concept
 *  Program 1 - CPU scheduling and FIFOs 
 * 
 *  Program runtime information is saved in shared memory.
 *  The information can be viewed by running prg_2.
 * 
 *  Compilation instruction: gcc -pthread -o prg_1 prg_1.c
 *  
 *  @author John Thai
 *  @date 2018-04-27 
 */ 

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Thread control struct
typedef struct{
    sem_t *one;
    sem_t *two; // Semaphore two control
    pthread_mutex_t *mutex; // Mutex lock
    pthread_attr_t *attr; // Set of thread attributes

}s_thread;

// pthread_mutex_t mutex; // Mutex lock
// pthread_attr_t attr; // Set of thread attributes

void thread_init(s_thread *t);
static void *thread_one(s_thread *t);
static void *thread_two(s_thread *t);

int main (int argc, char *argv[])
{
    // // Variable declare and define
    sem_t one, two; //Semaphore controls
    pthread_mutex_t mutex; // Mutex lock
    pthread_attr_t attr; // Set of thread attributes
    pthread_t tid1, tid2; // Thread IDs

    /* Struct setup */
    //s_thread t = {&one, &two, &mutex, &attr};
    //s_thread t;

    s_thread t = {&one, &two, &mutex, &attr};
    
    // Initialise threads and semaphores
    thread_init(&t);


    //if (pthread_attr_init(&attr)) // Initialise thread creation attributes
    //    handle_error("pthread_attr_init");


    /* Create threads */
    if (pthread_create(&tid1, &attr, (void *)thread_one, &t)) // Create thread one
        handle_error("pthread_create error1");
    if (pthread_create(&tid2, &attr, (void *)thread_two, &t)) // Create thread two
        handle_error("pthread_create error2");

    // Unlock semaphore one --This will allow thread one to run first
    if (sem_post(&one)) 
        handle_error("sem_post error");

    /* Wait for thread to exit */
    if (pthread_join(tid1, NULL))
        handle_error("pthread_join error");
    if (pthread_join(tid2, NULL))
        handle_error("pthread_join error");



    // Free memory allocated to thread
    if (pthread_attr_destroy(&attr))
        handle_error("pthread_attr_destroy error");
    if (pthread_mutex_destroy(&mutex))
        handle_error("pthread_mutex_destroy error");
    
    // Free memory allocated to semaphores
    sem_destroy(&one);
    sem_destroy(&two);

    exit(EXIT_SUCCESS);; // Program excuted all good!
}

void thread_init(s_thread *t)
{

    if (pthread_mutex_init(t->mutex, NULL)) 
        handle_error("pthread_mutex_init error");

    /* Create semaphores and block subroutine */
    if (sem_init(t->one, 0, 0))
        handle_error("sem_init error");
    if (sem_init(t->two, 0, 0))
        handle_error("sem_init error");

    // Initialise default attributes
    if (pthread_attr_init(t->attr)) 
        handle_error("pthread_attr_init error");

}

// thread 1
// Do sjf stuff 
static void *thread_one(s_thread *t)
{
    // s_thread *t; // Re-declare thread for private use

    if (sem_wait(t->one)) // Wait till unlocked
        handle_error("sem_wait error");
    if (pthread_mutex_lock(t->mutex)) // Lock mutex to prevent concurrent threads execution
        handle_error("pthread_mutex_lock error"); 

    // Do stuff here
    printf("In thread one");

    if (pthread_mutex_unlock(t->mutex)) // Release mutex lock
        handle_error("pthread_mutex_unlock error"); 
    if (sem_post(t->two)) // Release and unlock semaphore two
        handle_error("sem_post error");
}


//thread 2
// do mem stuff
static void *thread_two(s_thread *t)
{
    // s_thread *t; // Re-declare thread for private use

    if (sem_wait(t->two)) // Wait till unlocked
        handle_error("sem_wait error");
    if (pthread_mutex_lock(t->mutex)) // Lock mutex to prevent concurrent threads execution
        handle_error("pthread_mutex_lock error"); 

    // Do stuff here
    printf("In thread two");

    if (pthread_mutex_unlock(t->mutex)) // Release mutex lock
        handle_error("pthread_mutex_unlock error"); 
    if (sem_post(t->one)) // Release and unlock semaphore one
        handle_error("sem_post error");
}