/*! @file
 *  
 *  @brief A multi-threads program for read and writing data using pipe-line concept
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
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* Constant definition */
#define DATA_FILE "data.txt"
#define SRC_FILE "src.txt"
#define CONT_REG "end_header"
#define BUFFER_SIZE 1024

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

/* Defining structs */
typedef struct
{
    int isRead; // Variable to check pipe read status
    char buff[BUFFER_SIZE]; // Buffer to store reads
} struct_pipe; // Pipe data struct

typedef struct
{
    sem_t *write; // Pointer to write semaphore
    sem_t *read; // Pointer to write semaphore
    sem_t *justify; // Pointer to write semaphore
    pthread_mutex_t *mutex; // Mutex lock
    pthread_attr_t *attr; // Set of thread attributes
} s_thread; // Thread control stuct

typedef struct
{
    int *fd_read;
    int *fd_write; // Location of file data in memory to write
    FILE *data_fp; // File pointer to read from data.txt
    FILE *src_fp;  // File pointer to write to src.txt
    struct_pipe *pipe;
    s_thread *t;
    int *read_status; // Pointer to hold EOF status
    int *isContRegion; // Pointer to check data.txt file region
}s_data; // Thread data struct

/* Functions declaration */
void initData(s_thread *t); // Data initialisation function
static void *thread_start_a(s_data *d); // Tread A function
static void *thread_start_b(s_data *d); // Thread B function
static void *thread_start_c(s_data *d); // Thread C funtion
void shmwrite(double *time); // Shared memory writing function


int main(int argc, char *argv[])
{
    clock_t time_start = clock(); // Starting timer

    // Variables declare and define
    int read_status = 0; // End file status
    int isContRegion = 0; // Reading region status
    int fd[2]; // Pipe file descriptor
    pthread_t tidA, tidB, tidC; // Thread ID
    pthread_attr_t attr; // Set of thread attributes
    pthread_mutex_t mutex; // Mutex lock
    sem_t write; // Semaphore for writing
    sem_t read;  // Semaphroe for reading
    sem_t justify; // Semaphore for justifying

    // Piping
    if (pipe(fd) < 0)
        handle_error("pipe error");

    /* Opening files to read and write */
    FILE *data_fp = fopen(DATA_FILE, "r"); // "data.txt" file read
    if (data_fp == NULL){ // Error handling
        perror("fopen data_fp error");
        // Pipe status error handling
        if (fd[0]){
            if (close(fd[0])) 
                perror("fd error");
        }
        if (fd[1]){
            if (close(fd[1]))
                perror("fd error");
        }
        exit(EXIT_FAILURE);
    }

    FILE *src_fp = fopen(SRC_FILE, "w"); // "src.txt" file write
    if (src_fp == NULL){ //Error handling
        perror("fopen src_fp error");
        if (fclose(data_fp))
            perror("fclose data_fp error");
        exit(EXIT_FAILURE);
    }

    /* Setting up structs */
    struct_pipe pipe = {0};
    s_thread d = {&write, &read, &justify, &mutex, &attr};
    s_data s = {&fd[0], &fd[1], data_fp, src_fp, &pipe, &d, &read_status, &isContRegion};
    
    // Initialise threads and semaphores
    initData(&d); 

    // Iterate through threads sequentially till final read from data.txt file
    while (read_status != EOF){
        /* Create threads */
        if (pthread_create(&tidA, &attr, (void *)thread_start_a, &s)) // Create thread A
            handle_error("pthread_create error");
        if (pthread_create(&tidB, &attr, (void *)thread_start_b, &s)) // Create thread B
            handle_error("pthread_create error");
        if (pthread_create(&tidC, &attr, (void *)thread_start_c, &s)) // Create thread C
            handle_error("pthread_create error");
        
        // Unlock semaphore A --This will allow thread A to run first
        if (sem_post(&write)) 
            handle_error("sem_post error");

        /* Wait for thread to exit */
        if (pthread_join(tidA, NULL))
            handle_error("pthread_join error");
        if (pthread_join(tidB, NULL))
            handle_error("pthread_join error");
        if (pthread_join(tidC, NULL))
            handle_error("pthread_join error");
    }

    /* Cleaning and releasing resources */
    // Close read and write pipe
    close(fd[0]);
    close(fd[1]);

    // Close read and write file
    fclose(data_fp);
    fclose(src_fp);

    // Free memory allocated to thread
    if (pthread_attr_destroy(&attr))
        handle_error("pthread_attr_destroy error");
    if (pthread_mutex_destroy(&mutex))
        handle_error("pthread_mutex_destroy error");
    
    // Free memory allocated to semaphores
    sem_destroy(&write);
    sem_destroy(&read);
    sem_destroy(&justify);
    
    /* Runtime calculation */
    clock_t time_end = clock();
    double time_spent = (double)(time_end - time_start)/ CLOCKS_PER_SEC;
    
    // Write to shared memory
    shmwrite(&time_spent); 

    printf("Program execution completed!\nRuntime: %f second(s)\n", time_spent);

    exit(EXIT_SUCCESS); // Program excuted all good!
}

/*! @brief Initialises threads and semaphores
 *  
 *  @param d - A pointer to s_thread structure
 *  @return - void
 */
void initData(s_thread *d)
{
    // Create mutex lock
    if (pthread_mutex_init(d->mutex, NULL)) 
        handle_error("pthread_mutex_init error");

    /* Create semaphores and block subroutine */
    if (sem_init(d->write, 0, 0))
        handle_error("sem_init error");
    if (sem_init(d->read, 0, 0))
        handle_error("sem_init error");
    if (sem_init(d->justify, 0, 0))
        handle_error("sem_init error");

    // Initialise default attributes
    if (pthread_attr_init(d->attr)) 
        handle_error("pthread_attr_init error");
}

/*! @brief Writes one line of charater to pipe from selected data file
 *  
 *  @param s - A pointer to s_data structure
 *  @return - void
 */
static void *thread_start_a(s_data *s)
{
    char buff[BUFFER_SIZE]; // Local buffer to get data from data.txt file

    if (sem_wait(s->t->write)) // Wait till unlocked
        handle_error("sem_wait error");
    if (pthread_mutex_lock(s->t->mutex)) // Lock mutex to prevent concurrent threads execution
        handle_error("pthread_mutex_lock error"); 

    // Put character from data.txt into a temporary buffer
    if(fgets(buff, sizeof(buff), s->data_fp) == NULL) 
    {
        *s->read_status = EOF; // End of file flag
    }

    // Write data from buffer to file descriptor
    write(*s->fd_write, buff, strlen(buff)); 
    
    if (pthread_mutex_unlock(s->t->mutex)) // Release mutex lock
        handle_error("pthread_mutex_unlock error"); 
    if (sem_post(s->t->read)) // Release and unlock semaphore B 
        handle_error("sem_post error");
}

/*! @brief Reads data from pipe and pass data to be read by thread C
 *  
 *  @param s - A pointer to s_data structure
 *  @return - void
 */
static void *thread_start_b(s_data *s)
{
    if (sem_wait(s->t->read)) // Wait till unlocked
        handle_error("sem_wait error");
    if (pthread_mutex_lock(s->t->mutex)) // Lock mutex to prevent concurrent threads execution
        handle_error("pthread_mutex_lock error"); 

    // Reads data from pipe and pass data to thread C
    s->pipe->isRead = read(*s->fd_read, s->pipe->buff, BUFFER_SIZE);

    if (pthread_mutex_unlock(s->t->mutex)) // Release mutex lock
        handle_error("pthread_mutex_unlock error"); 
    if (sem_post(s->t->justify)) // Release and unlock semaphore C 
        handle_error("sem_post error");
}

/*! @brief Reads passed data and determine content data region then writes data from content region to file.
 *  
 *  @param s - A pointer to s_data structure
 *  @return - void
 */
static void *thread_start_c(s_data *s)
{
    if (sem_wait(s->t->justify)) // Wait till unlocked
        handle_error("sem_wait error");
    if (pthread_mutex_lock(s->t->mutex)) // Lock mutex to prevent concurrent threads execution
        handle_error("pthread_mutex_lock error"); 

    //Write data to src.txt
    if (*s->isContRegion)
    {
        fprintf(s->src_fp, "%.*s", s->pipe->isRead, s->pipe->buff); //print to file   
    }
    // Read passed data and determine data region by checking if beginning of content region
    if(!(strncmp(s->pipe->buff, CONT_REG, strlen(CONT_REG))))
    {
        *s->isContRegion = 1;
    }
    
    // Clear array to prevent memleaks
    memset(s->pipe->buff, 0, BUFFER_SIZE); 
    
    if (pthread_mutex_unlock(s->t->mutex)) // Release mutex lock
        handle_error("pthread_mutex_lock error"); 
    if (sem_post(s->t->write)) // Release and unlock semaphore A 
        handle_error("sem_post error");
}

/*! @brief Write data to memory to be read by another process
 *  
 *  @param s - A pointer to s_data structure
 *  @return - void
 */
void shmwrite(double *time)
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
    memset(p, '\0', sizeof(time)); // Clearing bits of data before transfering
    memcpy(p, time, sizeof(time)); // Writing to memory!
    
    /* Detach from memory */
    retval = shmdt(p);
    if (retval < 0)
        handle_error("Detachment failed");
}
