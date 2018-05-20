/*! @file
 *  
 *  @brief A multi-threads program that applies 
 *  CPU scheduling using Shortest-Remaining-Time-First (SRTF) algorithm
 *  and read/write data using FIFO concept
 * 
 *  CPU scheduling information is saved in CPU memory.
 *  The information can be viewed in "output.txt" file after execution.
 * 
 *  Compilation instruction: gcc -pthread -o prg_1 prg_1.c
 *  
 *  Acknowledgement: 
 *  SRTF method - https://tinyurl.com/y9zevrn9 
 *  
 *  @author John Thai
 *  @date 2018-05-20 
 */ 

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>


#define FIFONAME "/tmp/myfifo"
#define MAX_BUF 1024
#define OUT_FILE "output.txt"
#define MSG_BUF 100

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Thread control struct
typedef struct{
    sem_t *one;
    sem_t *two; // Semaphore two control
    pthread_mutex_t *mutex; // Mutex lock
    pthread_attr_t *attr; // Set of thread attributes
}s_thread;

// SRTF process struct
typedef struct{
    int pid; // Process ID
    int art; // Arrival time
    int bt; // Burst time
}s_process;

/* Functions declaration */
// Thread functions
void thread_init(s_thread *t);
static void *thread_one(s_thread *t);
static void *thread_two(s_thread *t);
// SRTF process functions
void wait_time(s_process proc[], int n, int wt[]);
void turnaround_time(s_process proc[], int n, int wt[], int tat[]);
char *average_time(s_process proc[], int n);


int main (int argc, char *argv[])
{
    // // Variable declare and define
    sem_t one, two; //Semaphore controls
    pthread_mutex_t mutex; // Mutex lock
    pthread_attr_t attr; // Set of thread attributes
    pthread_t tid1, tid2; // Thread IDs

    /* Struct setup */
    s_thread t = {&one, &two, &mutex, &attr};
    
    // Initialise threads and semaphores
    thread_init(&t);

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
// Do sjf stuff, write to FIFO
static void *thread_one(s_thread *t)
{
    int fd; // FIFO write file descriptor
    char *received_msg = malloc(MSG_BUF); // Allocate string for avg time message

    if (sem_wait(t->one)) // Wait till unlocked
        handle_error("sem_wait error");
    if (pthread_mutex_lock(t->mutex)) // Lock mutex to prevent concurrent threads execution
        handle_error("pthread_mutex_lock error"); 

    // Do stuff here
    printf("In thread one:\n");

    // Set up process data
    s_process proc[] = {{ 1, 8, 10 }, 
                        { 2, 10, 3 },
                        { 3, 14, 7 }, 
                        { 4,  9, 5 }, 
                        { 5, 16, 4 }, 
                        { 6, 21, 6 },
                        { 7, 26, 2 }};
        
    // Number of processes                    
    int n = sizeof(proc) / sizeof(proc[0]);
 
    // Call average time
    received_msg = average_time(proc, n);

    // Create the FIFO (named pipe)
    if(mkfifo(FIFONAME, 0666) < 0)
        handle_error("mkfifo error");

    /* Unlock mutex and sempost two since fifo require both thread to run simultanously */
    if (pthread_mutex_unlock(t->mutex)) // Release mutex lock
        handle_error("pthread_mutex_unlock error"); 
    // printf("unlocked mutex in thread 1!\n");

    if (sem_post(t->two)) // Release and unlock semaphore two
        handle_error("sem_post error");
    // printf("semposted 2!\n");

    // TODO: THERE MIGHT BE AN ERROR HERE!
    fd = open(FIFONAME, O_WRONLY); //Open FIFO for writing
        // handle_error("open error");

    write(fd, received_msg, MSG_BUF); // Write to FIFO
    
    close(fd); // Close the FIFO       
    if(unlink(FIFONAME) < 0) // Remove the FIFO 
        handle_error("unlink error");
}


//thread 2
// Read from fifo and write to file
static void *thread_two(s_thread *t)
{

    int fd; // FIFO read file descriptor
    char buf[MAX_BUF]; // Buffer to receive data from FIFO
    FILE *output_fp = fopen(OUT_FILE, "w"); // Pointer to write to file

    if (sem_wait(t->two)) // Wait till unlocked
        handle_error("sem_wait error");
    if (pthread_mutex_lock(t->mutex)) // Lock mutex to prevent concurrent threads execution
        handle_error("pthread_mutex_lock error"); 

    // Do stuff here
    printf("In thread two:\n");

    // Open, read, and display the message from the FIFO
    fd = open(FIFONAME, O_RDONLY); // Open FIFO for reading
    if(read(fd, buf, MAX_BUF) == 0) // Read from FIFO
        printf("FIFO Empty");
    else{
        printf("%s\n", buf);
        fprintf(output_fp, "%s", buf); // Write to "output.txt" file
    }

    fclose(output_fp); // Close file pointer
    close(fd); // Close FIFO


    if (pthread_mutex_unlock(t->mutex)) // Release mutex lock
        handle_error("pthread_mutex_unlock error"); 
    if (sem_post(t->one)) // Release and unlock semaphore one
        handle_error("sem_post error");
}

/* Function to find all processes wait time */
void wait_time(s_process proc[], int n, int wt[])
{
    int rt[n]; //Remaining time

    // Copy burst time into rt[]
    for(int i = 0; i < n; i++)
        rt[i] = proc[i].bt;

    int complete = 0, t = 0, min = INT_MAX; // Set min to max for later comparision
    int shortest = 0, finish_time;
    bool check = false;

    // Run till all process completed
    while(complete != n) {
        // Find process with minimum remaining time from all arrived processes till current time
        for(int j = 0; j < n; j++) {
            if((proc[j].art <= t) && (rt[j] < min) && rt[j] > 0)
            {
                min = rt[j];
                shortest = j;
                check = true;
            }
        }

        if (check == false){
            t++;
            continue;
        }

        rt[shortest]--; // Reduce remaining time

        // Update minimum
        min = rt[shortest];
        if (min == 0)
            min = INT_MAX;

        // If a process gets completely executed
        if (rt[shortest] == 0)
        {
            complete++; // Increment complete

            // Set finish time for current process
            finish_time = t+1;

            // Calculate waiting time
            wt[shortest] = finish_time - proc[shortest].bt - proc[shortest].art;
            
            if (wt[shortest] < 0)
                wt[shortest] = 0;
        }
        t++; // Increment time
    }
}
   
/* Function to calculate turnaround time */
void turnaround_time(s_process proc[], int n, int wt[], int tat[])
{
    // Calculate turnaround time
    for (int i = 0; i < n; i++)
        tat[i] = proc[i].bt + wt[i];
}

// Calculate average time for wait time and turnaround time then display results
// Return string of average time
char *average_time(s_process proc[], int n)
{
    int wt[n], tat[n], total_wt = 0, total_tat = 0;
    char *avg_time_msg = malloc(100);

    // Call wait_time function
    wait_time(proc, n, wt);

    // Call turnaround_time function
    turnaround_time(proc, n, wt, tat);

    // Display result headings
    printf("\tProcess ID\tArrival Time\tBurst Time\tWait Time\tTurnaround Time\n");

    // Calculate total wait time and turnaround time
    for(int i = 0; i < n; i++) {
        total_wt = total_wt + wt[i];
        total_tat = total_tat + tat[i];

        // Display individual component
        printf("\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", proc[i].pid, proc[i].art, proc[i].bt, wt[i], tat[i]);
    }

    float avg_wt = (float)total_wt/(float)n;
    float avg_tat = (float)total_tat/(float)n;
    printf("\nAverage wait time: %f", avg_wt);
    printf("\nAverage turnaround time: %f\n", avg_tat);
    sprintf(avg_time_msg, "Average waiting time: %f\nAverage turn-around time: %f\n"
            , avg_wt, avg_tat);	

    return avg_time_msg;
}
