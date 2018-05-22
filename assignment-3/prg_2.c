// Deadlock detection
// Compilation: gcc -o prg_2 prg_2.c -std=c99
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define DATA_FILE "Topic2_Prg_2.txt" 
#define OUT_FILE "output_topic2.txt"

#define NUM_PROC 9
#define NUM_RESOURCE 3

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

int request[NUM_PROC][NUM_RESOURCE];// = {{0,1,2},{2,0,2},{0,0,2},{3,2,2},{0,3,5},{0,1,1},{1,6,4},{5,0,3},{1,2,4}};
int alloc[NUM_PROC][NUM_RESOURCE];// = {{0,1,0},{2,0,0},{3,0,3},{2,1,1},{0,0,2},{2,1,3},{5,2,4},{1,3,1},{2,4,2}};
int avail[NUM_RESOURCE];

bool finish[NUM_PROC];
int completed_process[NUM_PROC]; // Array to store completed process
int incompleted_process[NUM_PROC]; // Array to store incompleted process
bool complete = false; // Flag to check if possible to continue running process after first run
int complete_count = 0; // Keep track complete_process array
int sequence_process[NUM_PROC];

void detector();
void readFile();
void requestCheck(int initial_resource, int initial_process);
void nameityourself(int sig);


int main(int argc, char *argv[])
{
	if (signal(SIGUSR1, nameityourself))
	{
		perror("signal");
		exit(0);
	}
	
	FILE *output_fp = fopen(OUT_FILE, "w"); // "src.txt" file write
  if (output_fp == NULL){ //Error handling
      perror("fopen output_fp error");
      if (fclose(output_fp))
          perror("fclose output_fp error");
      exit(EXIT_FAILURE);
	}
	
	readFile();
	detector();
	// No deadlock output sequence IDS
	if (complete_count == NUM_PROC)
	{
		printf("No Deadlock Occured.\nProcess sequence IDs: "); // Print to console
		fprintf(output_fp, "No Deadlock Occured.\nProcess sequence IDs: ");
		for (int i = 0; i < (NUM_PROC); i++)
		{
			printf("P%i\t", sequence_process[i]); // Print to console
			fprintf(output_fp, "P%i\t", sequence_process[i]);
		}
		printf("\n");
		
	}
	
	// Deadlock output deadlock process
	if (complete_count < NUM_PROC)
	{
		printf("Deadlock Occured.\nDeadlocked process IDs: ");
		fprintf(output_fp, "No Deadlock Occured.\nProcess sequence IDs: ");
		for (int j = 0; j < (NUM_PROC); j++)
		{
			//printf("P%i\t", sequence_process[j]);
			if (!(finish[j]))
			{
				printf("P%d\t", j);
				fprintf(output_fp, "P%i\t", j);
			}
		}
		printf("\n");
	}
	
	kill(getpid(), SIGUSR1);
	
	return 0;
}
// Read from file
void readFile()
{
	//int lineNumber = 4;
	//int count = 0;
	char line[256]; // Holder
	FILE *proc_fp = fopen(DATA_FILE, "r");
  if (proc_fp == NULL){ //Error handling
    perror("fopen src_fp error");
    if (fclose(proc_fp))
        perror("fclose data_fp error");
		exit(EXIT_FAILURE);
	}
	
	// Scan the first 2 lines
	fgets(line, sizeof(line), proc_fp);
	puts(line);
	fgets(line, sizeof(line), proc_fp);
	puts(line);
	
	// Scan first process including available
	fscanf(proc_fp, "%s %i %i %i %i %i %i %i %i %i", line, &alloc[0][0], &alloc[0][1], &alloc[0][2], &request[0][0], &request[0][1], &request[0][2], &avail[0], &avail[1], &avail[2]);
	// TEST
	printf("%s          %i %i %i       %i %i %i       %i %i %i\n", line, alloc[0][0], alloc[0][1], alloc[0][2], request[0][0], request[0][1], request[0][2], avail[0], avail[1], avail[2]);
	
	// Scan the rest of the processes
	for (int i = 1; i < NUM_PROC; i++)
	{
		fscanf(proc_fp, "%s %i %i %i %i %i %i", line, &alloc[i][0], &alloc[i][1], &alloc[i][2], &request[i][0], &request[i][1], &request[i][2]);
		//TEST
		printf("%s          %i %i %i       %i %i %i\n", line, alloc[i][0], alloc[i][1], alloc[i][2], request[i][0], request[i][1], request[i][2]);
	}
	
	
	fclose(proc_fp);
}


// Detect deadlock
// Loop through till no-deadlock or till process being try count twice (being looped 2times continuously?)
void detector ()
{	
	/* Check for empty allocation resources in all processes */
	for (int i = 0; i < NUM_PROC; i++)
	{ // Loop through processes
		int sum_resource = 0; // Varaible to keep track of each resource sum
		for (int j = 0; j < NUM_RESOURCE; j++) // Loop through resources
			sum_resource += alloc[i][j];
		if (sum_resource == 0)
			finish[i] = true; // Set finish to true if no resource in alloc
		else
			finish[i] = false; // Set finish to false if resource available in alloc
	}
	
	
	int initial_resource = NUM_RESOURCE;
	int initial_process = NUM_PROC;
	int incomplete_count = 0;
	int sequence = 0; // Variable to check the current process sequence
	bool lesquest[initial_process]; // bool for request < work
	
	//Set available resource to work
	int work[NUM_RESOURCE];
	for (int i = 0; i < initial_resource; i++)
		work[i] = avail[i];
		
	
	
	// check if any finished is false and hasn't been loop through twice
	// or set a global incomplete flag and the same value has not been loop over twice continuously?
	while(complete == false)
	{
		// Check if request is less than work for all false finish */
		for (int i = 0; i < initial_process; i++) // Loop through each process
		{
			int count = 0; // Counter to compare request and work
			//Check if request is less than work for all false finish
			while (count < initial_resource && finish[i] == false) // Loop through each resource
			{
				if(request[i][count] <= work[count]) // Comparing each resource
				{
					lesquest[i] = true;
					count++; // Increment count to compare next value	
				}
				else
				{
					lesquest[i] = false;
					count = 0; // Reset count
					break; // Break out of loop and move to next process
				}
			} // End of while loop
		
			// Process 3
			if (lesquest[i] && finish[i] == false) //Check if request < work and finish is already false
			{
				//printf("work: "); // TEST
				for(int j = 0; j < initial_resource; j++)
				{
					work[j] += alloc[i][j];
					//printf("%d ",work[j]); //TEST
				}
				
				finish[i] = true;
				// Save completed process id
				sequence_process[sequence] = i; // Store everything in order
				sequence++;
				
				printf("Completed process P%d\n", i);
				complete_count++;
				if (complete_count == NUM_PROC)
					complete = true;
				if (incomplete_count > 1)
					incomplete_count--;
			}
			// Check for false finish and havent been looped check 
			else if(!(lesquest[i]) && finish[i] == false && incomplete_count <= 1)
			{
				incomplete_count++;
				//printf("Incompleted\n");
				complete = false;
			}
			// Check for looped check
			else if(incomplete_count > 1){
				//printf("looped\n");
				incomplete_count--;
				complete = true;
			}
			
		} // End of for loop	
	}
	//TODO: Gotta do something with continueing after first run
	// What happen when there is a deadlock
	printf("Complete count: %i\n", complete_count);
}

void nameityourself(int sig)
{
	if (sig == (SIGUSR1 || SIGUSR2))
		printf("Writing to output_topic2.txt has finished\n");
}


// Output process IDs sequence


// Output process ID where deadlock occured if true


// Write to file "output_topic2.txt"


// Send user SIGUSR1? after program completed
// -- oh lol its just a line "Writing to output_topic2.txt has finished"
// maybe not lol

// Generate gantt-chart for the lols
