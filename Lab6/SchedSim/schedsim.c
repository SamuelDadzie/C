// C program for implementation of Simulation 
#include<stdio.h> 
#include<limits.h>
#include<stdlib.h>
#include "process.h"
#include "util.h"


// Function to find the waiting time for all  
// processes - Round Robin
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    /* Create array to track remaining burst times */
    int rem_bt[n];
    for (int i = 0; i < n; i++)
    {
        rem_bt[i]   = plist[i].bt;  /* copy burst times */
        plist[i].wt = 0;            /* initialize waiting times to 0 */
    }

    int t = 0;  /* current time */

    /* Keep traversing until all processes are done */
    while (1)
    {
        int done = 1;  /* assume all done */

        for (int i = 0; i < n; i++)
        {
            if (rem_bt[i] > 0)
            {
                done = 0;  /* there is a pending process */

                if (rem_bt[i] > quantum)
                {
                    /* Process needs more than one quantum */
                    t += quantum;
                    rem_bt[i] -= quantum;
                }
                else
                {
                    /* Last cycle for this process */
                    t += rem_bt[i];
                    plist[i].wt = t - plist[i].bt;
                    rem_bt[i] = 0;  /* process is done */
                }
            }
        }

        if (done == 1)
            break;
    }
} 

// Function to find the waiting time for all  
// processes - Shortest Job First (SRTF preemptive)
void findWaitingTimeSJF(ProcessType plist[], int n)
{ 
    /* Array to track remaining burst times */
    int rem_bt[n];
    for (int i = 0; i < n; i++)
        rem_bt[i] = plist[i].bt;

    int complete = 0;   /* number of completed processes */
    int t        = 0;   /* current time */
    int min_bt   = INT_MAX;
    int shortest = 0;
    int finish_time;
    int found = 0;

    /* Traverse until all processes are complete */
    while (complete != n)
    {
        /* Find process with minimum remaining burst time */
        min_bt = INT_MAX;
        found  = 0;
        for (int j = 0; j < n; j++)
        {
            if (plist[j].art <= t && rem_bt[j] < min_bt && rem_bt[j] > 0)
            {
                min_bt   = rem_bt[j];
                shortest = j;
                found    = 1;
            }
        }

        if (!found)
        {
            t++;
            continue;
        }

        /* Reduce remaining burst time by 1 */
        rem_bt[shortest]--;

        /* If process is complete */
        if (rem_bt[shortest] == 0)
        {
            complete++;
            finish_time = t + 1;

            /* Calculate waiting time */
            plist[shortest].wt = finish_time - plist[shortest].art
                                             - plist[shortest].bt;

            /* Waiting time can't be negative */
            if (plist[shortest].wt < 0)
                plist[shortest].wt = 0;
        }

        t++;
    }
} 

// Function to find the waiting time for all  
// processes - FCFS
void findWaitingTime(ProcessType plist[], int n)
{ 
    /* waiting time for first process is 0 (or arrival time) */
    plist[0].wt = 0 + plist[0].art; 
  
    /* calculating waiting time for remaining processes */
    for (int i = 1; i < n; i++) 
        plist[i].wt = plist[i-1].bt + plist[i-1].wt; 
} 
  
// Function to calculate turn around time 
void findTurnAroundTime(ProcessType plist[], int n)
{ 
    /* turnaround time = burst time + waiting time */
    for (int i = 0; i < n; i++) 
        plist[i].tat = plist[i].bt + plist[i].wt; 
} 
  
// Comparator function to sort processes by priority (highest first)
int my_comparer(const void *this, const void *that)
{ 
    /* Cast to ProcessType pointers */
    ProcessType *a = (ProcessType *)this;
    ProcessType *b = (ProcessType *)that;

    /* Sort in ascending order of priority (lower number = higher priority) */
    if (a->pri < b->pri) return -1;
    if (a->pri > b->pri) return  1;
    return 0;
} 

// Function to calculate average time - FCFS
void findavgTimeFCFS(ProcessType plist[], int n) 
{ 
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nFCFS\n");
}

// Function to calculate average time - SJF
void findavgTimeSJF(ProcessType plist[], int n) 
{ 
    findWaitingTimeSJF(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nSJF\n");
}

// Function to calculate average time - Round Robin
void findavgTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    findWaitingTimeRR(plist, n, quantum); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

// Function to calculate average time - Priority
void findavgTimePriority(ProcessType plist[], int n) 
{ 
    /* Step 1: Sort processes by priority using qsort */
    qsort(plist, n, sizeof(ProcessType), my_comparer);

    /* Step 2: Apply FCFS algorithm on sorted list */
    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);

    printf("\n*********\nPriority\n");
}

void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
  
    for (int i = 0; i < n; i++) 
    { 
        total_wt  += plist[i].wt; 
        total_tat += plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", 
               plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt  / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

ProcessType * initProc(char *filename, int *n) 
{
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}
  
// Driver code 
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;

    ProcessType *proc_list;
  
    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        fflush(stdout);
        return 1;
    }
    
    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);
  
    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n); 
    printMetrics(proc_list, n);
  
    // Priority
    n = 0; 
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n); 
    printMetrics(proc_list, n);
    
    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum); 
    printMetrics(proc_list, n);
    
    return 0; 
}
