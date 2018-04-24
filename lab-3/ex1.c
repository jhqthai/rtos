/*****
 * Compilation guide: gcc -o 
 * 
 * 
 ****/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int sum;/*this data is shared by the threads*/
/* The mutex lock */
pthread_mutex_t mutex;
/* the semaphores */
sem_t one, two;
pthread_t tid1,tid2;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void *runnerOne(void *param);/*threads call this function*/
void *runnerTwo(void *param);/*threads call this function*/
void initializeData();

int main(int argc, char*argv[])
{
 // pthread_t tid;/*the thread identifier*/
 // pthread_attr_t attr;/*set of thread attributes*/

  if(argc!=2){
	fprintf(stderr,"usage: a.out <integer value>\n");
	return -1;
  }
  if(atoi(argv[1])<0){
	fprintf(stderr,"%d must be >=0\n",atoi(argv[1]));
	return -1;
  }
  initializeData();

  /*get the default attributes*/
  pthread_attr_init(&attr);
  
  /*create the thread 1*/
  pthread_create(&tid1,&attr,runnerOne,argv[1]);
  
  printf("sum=%d\n",sum);

  /*create the thread 2*/
  // add your program 
  pthread_create(&tid2, &attr, runnerTwo, argv[1]);
  
  /*send semaphore to thread 2 and begin the threads running*/
  sem_post(&two);
  /*wait for the thread to exit*/
  // add your program
  pthread_join(tid1, NULL);
  pthread_join(tid2,NULL); 
  
  printf("sum=%d\n",sum);

}

/*The thread will begin control in this function*/
void *runnerOne(void *param)
{
  /* wait for the semaphore one */
  sem_wait(&one);
  
  /* mutex lock the program */
  pthread_mutex_lock(&mutex);
  
  /* calculation */
  int i,upper=atoi(param);
  
  for(i=1;i<=upper;i++)
    sum+=i; 
  
  printf("thread one\n");
  /* release the mutex lock */
  pthread_mutex_unlock(&mutex); 
  
  /* send semaphore to thread 2*/
  sem_post(&two);
  

}

/*The thread will begin control in this function*/
void *runnerTwo(void *param)
{
  /* wait for the semaphore two */
  // add your program 
  sem_wait(&two);
  
  /* mutex lock the program */
  // add your program 
  pthread_mutex_lock(&mutex);

  /* calculation */
  // add your program   
  int i, upper=atoi(param);

  for(i=-100;i<=upper;i++);
    sum+=1;
  
  printf("thread two\n");
  /* release the mutex lock */
  // add your program 
  pthread_mutex_unlock(&mutex);

  /* send semaphore to thread 1*/
  // add your program 
  sem_post(&one);


}

void initializeData() {
   sum=0;
   
   /* Create the mutex lock */
   pthread_mutex_init(&mutex, NULL);

   /* Create the one semaphore and initialize to 0 */
   sem_init(&one, 0, 0);

   /* Create the two semaphore and initialize to BUFFER_SIZE */
   sem_init(&two, 0, 0);

   /* Get the default attributes */
   pthread_attr_init(&attr);
}
