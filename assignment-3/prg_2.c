/*! @file
 *  
 *  @brief A deadlock detection program uses value from provided text file.
 *  
 *  The information can be viewed on console 
 *  and in "output_topic2.txt" after excution
 * 
 *  Note: Include "Topic2_Prg_2.txt" file in the same folder to be read
 *
 *  Compilation instruction: gcc -o prg_2 prg_2.c -std=c99
 *  
 * 	Acknowledgement: Emily Tiang - Easy simple file reading concept
 * 
 *  @author John Thai
 *  @date 2018-05-22
 */ 
 
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#define DATA_FILE "Topic2_Prg_2.txt" // Name of file to read
#define OUT_FILE "output_topic2.txt" // Name of file to write

#define NUM_PROC 9  // Number of processes
#define NUM_RESOURCE 3 // Number of resources

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Array to store values read from file
int request[NUM_PROC][NUM_RESOURCE];
int alloc[NUM_PROC][NUM_RESOURCE];
int avail[NUM_RESOURCE];

bool finish[NUM_PROC]; // Array of bool for finish status flag
int complete_count; // Keep track complete processes
int sequence_process[NUM_PROC]; // Array to store processes in sequence

// Functions declaration
void detector(); // Function to detect deadlock
void readFile(); // Function to read from file
void signalHandler(int sig); // User signal handler


int main(int argc, char *argv[])
{
  // Initialising signal
	if (signal(SIGUSR1, signalHandler))
		handle_error("signal");
	
	// Open file to write
	FILE *output_fp = fopen(OUT_FILE, "w");
  	if (output_fp == NULL){ //Error handling
		perror("fopen output_fp error");
		if (fclose(output_fp))
			perror("fclose output_fp error");
		exit(EXIT_FAILURE);
	}
	
	// Call function to read from file
	readFile();
	
	// Call function for deadlock detection
	detector();
	
	// Check if no deadlock then output sequence IDs
	if (complete_count == NUM_PROC)
	{
		printf("No Deadlock Occured.\nSequence process IDs: "); // Print to console
		fprintf(output_fp, "No Deadlock Occured.\nSequence process IDs: "); // Write to file
		for (int i = 0; i < (NUM_PROC); i++) // Loop through processes
		{
			printf("P%i\t", sequence_process[i]); // Print process id to console
			fprintf(output_fp, "P%i\t", sequence_process[i]); // Write process id to file
		}
		printf("\n");
		
	}
	
	// Check if deadlock then output deadlock sequence IDs
	if (complete_count < NUM_PROC)
	{
		printf("Deadlock Occured!\nDeadlock process ID(s): ");
		fprintf(output_fp, "Deadlock Occured!\nDeadlock process ID(s): ");
		for (int j = 0; j < (NUM_PROC); j++) // Loop through processes
		{
			if (!(finish[j])) // Check if process is finished
			{
				printf("P%i\t", j); // Print process id to console
				fprintf(output_fp, "P%i\t", j); // Write process id to file
			}
		}
		printf("\n");
	}
	
	kill(getpid(), SIGUSR1); // Trigger signal
	
	return 0;
}


/*! @brief Read data from file and pass data into appropriate array
 *  
 *  @return - void
 */
void readFile()
{
	char line[256]; // Array to hold characters
	
	// Open file to read
	FILE *proc_fp = fopen(DATA_FILE, "r");
  	if (proc_fp == NULL){ //Error handling
		perror("fopen proc_fp error");
		if (fclose(proc_fp))
			perror("fclose proc_fp error");
		exit(EXIT_FAILURE);
	}
	
	// Scan the first 2 lines to get it out of the way
	fgets(line, sizeof(line), proc_fp);
	fgets(line, sizeof(line), proc_fp);
	
	// Scan first process including available
	fscanf(proc_fp, "%s %i %i %i %i %i %i %i %i %i", line, &alloc[0][0], &alloc[0][1], &alloc[0][2], &request[0][0], &request[0][1], &request[0][2], &avail[0], &avail[1], &avail[2]);
	
	// Scan the rest of the processes
	for (int i = 1; i < NUM_PROC; i++)
		fscanf(proc_fp, "%s %i %i %i %i %i %i", line, &alloc[i][0], &alloc[i][1], &alloc[i][2], &request[i][0], &request[i][1], &request[i][2]);
	
	if (fclose(proc_fp)) // Close file
		handle_error("fclose proc_fp error");
}


/*! @brief Detect deadlock
 *  
 *  Detect deadlock and stores process sequence with or without deadlock
 *  @return - void
 */
void detector ()
{		
	// Variables delcaration
	int initial_resource = NUM_RESOURCE;
	int initial_process = NUM_PROC;
	int safety_count = 0; // Stores number of loop for multi-round check
	int sequence = 0; // Current sequence process
	bool lesquest[initial_process]; // Bool for request < work
	
	//Set available resource to work
	int work[NUM_RESOURCE];
	for (int i = 0; i < initial_resource; i++)
		work[i] = avail[i];
		
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
		
	// Multi-loop safety check for deadlock scenario
	while(safety_count < NUM_PROC)
	{
		// Check if request is less than work for all false finish */
		for (int i = 0; i < initial_process; i++) // Loop through each process
		{
			int count = 0; // Counter to compare request and work
			//Check if request is less than work for all false finish
			while (count < initial_resource && finish[i] == false)
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
		
			//Check if request < work and finish is already false
			if (lesquest[i] && finish[i] == false) 
			{
				// Compute new work
				for(int j = 0; j < initial_resource; j++)
				{
					work[j] += alloc[i][j];
					//printf("%d ",work[j]); //TEST
				}
				
				finish[i] = true;
				
       			// Store everything in order
				sequence_process[sequence] = i; 
				sequence++;
				
			}			
		} // End of for loop	
		safety_count++; // Increment count
	}
	complete_count = sequence; // Set sequence to global complete count
}
/*! @brief Handle user defined signal
 *  
 *  @param sig - signal number to triggered
 *  @return - void
 */
void signalHandler(int sig)
{
	// Check if signal is received
	if (sig == SIGUSR1)
		printf("Writing to output_topic2.txt has finished.\n");
}

