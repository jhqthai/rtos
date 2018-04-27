/******
 * Instruction:
 * 
 * 
 * *****/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* Constant definition */
#define DATA_FILE "data.txt"
#define SRC_FILE "src.txt"
#define CONT_REG "end_header"
#define BUFFER_SIZE 1024

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)


/* Defining structs */
typedef struct
{
    sem_t *write; 
    sem_t *read; 
    sem_t *justify; // Semaphores
    pthread_mutex_t *mutex; // Mutex lock
    pthread_attr_t *attr; // Set of thread attributes
} s_data; // Thread control stuct

typedef struct
{
    int isRead;
    char buff[BUFFER_SIZE]; // Buffer to store reads
} struct_pipe; // Pipe data struct

typedef struct
{
    int *fd_read;
    int *fd_write; // Location of file data in memory to write
    FILE *data_fp; // File pointer to read from data.txt
    FILE *src_fp;  // File pointer to write to src.txt
    struct_pipe *pipe;
    s_data *d;
    int *read_status; // Pointer to hold EOF status
    int *isContRegion; // Pointer to check data.txt file region
}s_thread; // Thread data struct

static void *thread_start_a(s_thread *t); // Tread A function
static void *thread_start_b(s_thread *t); // Thread B function
static void *thread_start_c(s_thread *t); // Thread C funtion
void initData(s_data *d);

int main(int argc, char *argv[])
{
    clock_t time_start = clock(); // Starting timer

    // Variables declare and define
    int read_status = 0;
    int isContRegion = 0;
    int fd[2]; // Pipe file descriptor
    pthread_t tidA, tidB, tidC; // Thread ID
    pthread_attr_t attr; // Set of thread attributes
    pthread_mutex_t mutex; // Mutex lock
    sem_t write; 
    sem_t read; 
    sem_t justify; // Semaphores

    // Piping
    if (pipe(fd)<0)
        handle_error("pipe error");

    /* Opening files to read and write */
    FILE *data_fp = fopen(DATA_FILE, "r"); // "data.txt" file read
    // Error handling
    if (data_fp == NULL){
        perror("fopen data_fp error");
        // Pipe status error handling
        if (fd[0]) {
            if (close(fd[0])) 
                perror("fd error");
        }
        if (fd[1]) {
            if (close(fd[1]))
                perror("fd error");
        }
        exit(EXIT_FAILURE);
    }

    FILE *src_fp = fopen(SRC_FILE, "w"); // "src.txt" file write
    //Error handling
    if (src_fp == NULL){
        perror("fopen src_fp error");
        if (fclose(data_fp))
            perror("fclose data_fp error");
        exit(EXIT_FAILURE);
    }

    /* Setting up structs */
    s_data d = {&write, &read, &justify, &mutex, &attr};
    struct_pipe pipe = {0}; // UHHHHHHHHHHH  
    s_thread s = {&fd[0], &fd[1], data_fp, src_fp, &pipe, &d, &read_status, &isContRegion};
    
    // Initialise threads and semaphores
    initData(&d); 


    // Iterate through threads concurrently till final read from data.txt file
    while (read_status != EOF){
        /* Create threads */
        pthread_create(&tidA, &attr, (void *)thread_start_a, &s); // Create thread A
        pthread_create(&tidB, &attr, (void *)thread_start_b, &s); // Create thread B
        pthread_create(&tidC, &attr, (void *)thread_start_c, &s); // Create thread C

        sem_post(&write); // Unlock semaphore A --This will allow thread A to run first
        
        /* Wait for thread to exit */
        pthread_join(tidA, NULL);
        pthread_join(tidB, NULL);
        pthread_join(tidC, NULL);
    }

    //close read and write pipe
    close(fd[0]);
    close(fd[1]);

    //probably needa close read and write file somewhere
    fclose(data_fp);
    fclose(src_fp);
    
    clock_t time_end = clock();
    double time_spent = (double)(time_end - time_start)/ CLOCKS_PER_SEC;
    printf("Runtime: %f\n", time_spent);
    exit(EXIT_SUCCESS); // Program excuted all good!
}

void initData(s_data *d)
{
    pthread_mutex_init(d->mutex, NULL); // Create mutex lock

    sem_init(d->write, 0, 0); // Create semaphore A and block subroutine
    sem_init(d->read, 0, 0); // Create semaphore B and block subroutine
    sem_init(d->justify, 0, 0); // Create semaphore C and block subroutine

    pthread_attr_init(d->attr); // Initialise default attributes
}

static void *thread_start_a(s_thread *s)
{
    char buff[BUFFER_SIZE];
    sem_wait(s->d->write); // Wait till unlocked
    pthread_mutex_lock(s->d->mutex); // Lock mutex to prevent concurrent threads execution

    if(fgets(buff, sizeof(buff), s->data_fp) == NULL) // Put character from data.txt into a temporary buffer
    {
        *s->read_status = EOF; // End of file flag
    }

    //Write data to pipe
    write(*s->fd_write, buff, strlen(buff)); // Write character from buffer to file descriptor (memory)
    
    //TEST
    printf("Thread A\n");
    printf("from fgets: %s\n", buff);
    // printf("Fd_write: %i\n", *s->fd_write);
    // printf("Fd_read: %i\n", *s->fd_read);
    pthread_mutex_unlock(s->d->mutex); // Release mutex lock
    sem_post(s->d->read); // Release and unlock semaphore B 

}

static void *thread_start_b(s_thread *s)
{
    sem_wait(s->d->read); // Wait till unlocked
    pthread_mutex_lock(s->d->mutex); // Lock mutex to prevent concurrent threads execution

    // Reads data from pipe and pass data to thread C
    s->pipe->isRead = read(*s->fd_read, s->pipe->buff, BUFFER_SIZE);


    //TEST
    printf("Thread B\n");
    printf("Read out: %s\n", s->pipe->buff);
    pthread_mutex_unlock(s->d->mutex); // Release mutex lock
    sem_post(s->d->justify); // Release and unlock semaphore C 

}

static void *thread_start_c(s_thread *s)
{
    sem_wait(s->d->justify); // Wait till unlocked
    pthread_mutex_lock(s->d->mutex); // Lock mutex to prevent concurrent threads execution

    //test
    printf("Thread C\n");
    
    //Write data to src.txt
    if (*s->isContRegion)
    {
        printf("Write to file.\n");
        fprintf(s->src_fp, "%.*s", s->pipe->isRead, s->pipe->buff); //print to file   
    }
    // Read passed data and determine data region by checking if beginning of content region
    if(!(strncmp(s->pipe->buff, CONT_REG, strlen(CONT_REG))))
    {
        *s->isContRegion = 1;
        printf("ENDHEADER EQUAL!\n");
    }
    

    //TEST
    printf("Read passed data: %s\n", s->pipe->buff);
    memset(s->pipe->buff, 0, BUFFER_SIZE); // Clear array to prevent memleaks?
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    pthread_mutex_unlock(s->d->mutex); // Release mutex lock
    sem_post(s->d->write); // Release and unlock semaphore A 
}
