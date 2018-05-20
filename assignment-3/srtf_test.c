#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

// SRTF process struct
typedef struct{
    int pid; // Process ID
    int art; // Arrival time
    int bt; // Burst time
}s_process;

void wait_time(s_process proc[], int n, int wt[]);
void turnaround_time(s_process proc[], int n, int wt[], int tat[]);
void average_time(s_process proc[], int n);

int main(int argc, char *argv[]){
    s_process proc[] = { { 1, 1, 6 }, { 2, 1, 8 },
                    { 3, 2, 7 }, { 4, 3, 3 } };
    int n = sizeof(proc) / sizeof(proc[0]);
 
    average_time(proc, n);

    return 0;
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

// Calculate average time for wait time and turnaround time
void average_time(s_process proc[], int n)
{
    int wt[n], tat[n], total_wt = 0, total_tat = 0;

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
    printf("\nAverage wait time: %f", (float)total_wt/(float)n);
    printf("\nAverage turnaround time: %f\n", (float)total_tat/(float)n);
}
