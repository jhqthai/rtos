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

/* Global variables delcaration */
pthread_mutex_t mutex; // Mutex lock
//sem_t write, read, justify; // Semaphores
pthread_t tidA, tidB, tidC; // Thread ID
pthread_attr_t attr; // Set of thread attributes
int read_status;
int isContRegion;

//TODO: Probably have to set up pipe stuff here?
typedef struct
{
    int isRead;
    char buff[BUFFER_SIZE]; // Buffer to store reads
} struct_pipe;

//TODO: something with struct and datafile
typedef struct
{
    sem_t *write;
    sem_t *read;
    int *fd_read;
    int *fd_write; // Location of file data in memory to write
    FILE *fp; // File pointer to read from data.txt
} struct_a;

typedef struct
{
    sem_t *read;
    sem_t *justify;
    int *fd_read;
    int *fd_write; // Location of file data in memory to read
    struct_pipe *pipe;
} struct_b;

typedef struct
{
    sem_t *justify;
    sem_t *write;
    struct_pipe *pipe;
    FILE *fp; // File pointer to write to src.txt
} struct_c;


static void *thread_start_a(struct_a *s); // Tread A function
static void *thread_start_b(struct_b *s); // Thread B function
static void *thread_start_c(struct_c *s); // Thread C funtion
void initData();

int main(int argc, char *argv[])
{
    clock_t time_start = clock(); // Starting timer

    isContRegion = 0;
    
    /* Initialise pipe */
    int fd[2];
    // Error handling
    if (pipe(fd)<0)
        handle_error("pipe error");

    /* Opening file */
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

    // Setups
    sem_t *write, *read, *justify; // Semaphores

    //Structs setups
    struct_pipe pipe = {0}; // UHHHHHHHHHHH    
    struct_a a = {write, read, &fd[0], &fd[1], data_fp};
    struct_b b = {read, justify, &fd[0], &fd[1], &pipe};
    struct_c c = {justify, write, &pipe, src_fp};


    // Initialise threads and semaphores
    pthread_mutex_init(&mutex, NULL); // Create mutex lock

    sem_init(a.write, 0, 0); // Create semaphore A and block subroutine
    sem_init(b.read, 0, 0); // Create semaphore B and block subroutine
    sem_init(c.justify, 0, 0); // Create semaphore C and block subroutine

    pthread_attr_init(&attr); // Initialise default attributes



    while (read_status != EOF){
        // TODO: Passing the correct param to thread
        /* Create threads */
        pthread_create(&tidA, &attr, (void *)thread_start_a, &a); // Create thread A
        pthread_create(&tidB, &attr, (void *)thread_start_b, &b); // Create thread B
        pthread_create(&tidC, &attr, (void *)thread_start_c, &c); // Create thread C

        sem_post(write); // Unlock semaphore A --This will allow thread A to run first
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
    
    clock_t time_end = clock();
    double time_spent = (double)(time_end - time_start)/ CLOCKS_PER_SEC;
    printf("Runtime: %f\n", time_spent);
    exit(EXIT_SUCCESS); // Program excuted all good!
}

// void initData()
// {
//     pthread_mutex_init(&mutex, NULL); // Create mutex lock

//     sem_init(&write, 0, 0); // Create semaphore A and block subroutine
//     sem_init(&read, 0, 0); // Create semaphore B and block subroutine
//     sem_init(&justify, 0, 0); // Create semaphore C and block subroutine

//     pthread_attr_init(&attr); // Initialise default attributes
// }

static void *thread_start_a(struct_a *s)
{
    char buff[BUFFER_SIZE];
    sem_wait(s->write); // Wait till unlocked
    pthread_mutex_lock(&mutex); // Lock mutex to prevent concurrent threads execution

    if(fgets(buff, sizeof(buff), s->fp) == NULL) // Put character from data.txt into a temporary buffer
    {
        read_status = EOF;
    }
    //close(*s->fd_read);
    //Write data to pipe
    write(*s->fd_write, buff, strlen(buff)); // Write character from buffer to file descriptor (memory)
    
    //TEST
    printf("Thread A\n");
    printf("from fgets: %s\n", buff);
    // printf("Fd_write: %i\n", *s->fd_write);
    // printf("Fd_read: %i\n", *s->fd_read);
    pthread_mutex_unlock(&mutex); // Release mutex lock
    sem_post(s->read); // Release and unlock semaphore B 

}

static void *thread_start_b(struct_b *s)
{
    sem_wait(s->read); // Wait till unlocked
    pthread_mutex_lock(&mutex); // Lock mutex to prevent concurrent threads execution

    //close(*s->fd_write);
    //TODO: reads data from pipe
    //TODO: pass data to thread C
    s->pipe->isRead = read(*s->fd_read, s->pipe->buff, BUFFER_SIZE);


    //TEST
    printf("Thread B\n");
    printf("Read out: %s\n", s->pipe->buff);
    pthread_mutex_unlock(&mutex); // Release mutex lock
    sem_post(s->justify); // Release and unlock semaphore C 

}

static void *thread_start_c(struct_c *s)
{
    sem_wait(s->justify); // Wait till unlocked
    pthread_mutex_lock(&mutex); // Lock mutex to prevent concurrent threads execution

    //test
    printf("Thread C\n");
    
    //TODO: read passed data
    //TODO: Determine data region
    if (isContRegion)
    {
        printf("Write to file.\n");
        fprintf(s->fp, "%.*s", s->pipe->isRead, s->pipe->buff); //print to file   
    }
    // Check if beginning of content region
    if(!(strncmp(s->pipe->buff, CONT_REG, strlen(CONT_REG))))
    {
        isContRegion = 1;
        printf("ENDHEADER EQUAL!\n");
    }
    //TODO: write data to src.txt

    //TEST
    
    printf("Read passed data: %s\n", s->pipe->buff);
    memset(s->pipe->buff, 0, BUFFER_SIZE); // Clear array to prevent memleaks?
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    pthread_mutex_unlock(&mutex); // Release mutex lock
    sem_post(s->write); // Release and unlock semaphore A 
}
