/* timer.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int handled     = 0;   // flag for SIGALRM
int alarm_count = 0;   // counts how many SIGALRM signals have fired
time_t start_time;     // records when the program started

void alrm_handler(int signum)
{
    printf("Hello World!\n");
    alarm_count++;     // track each alarm
    handled = 1;
    alarm(1);          // reschedule every 1 second
}

void int_handler(int signum)
{
    time_t end_time = time(NULL);
    printf("\nProgram ran for %ld second(s)\n", (long)(end_time - start_time));
    printf("Total alarms fired: %d\n", alarm_count);
    exit(0);
}

int main(int argc, char * argv[])
{
    start_time = time(NULL);           // record start time

    signal(SIGALRM, alrm_handler);    // handle periodic alarm
    signal(SIGINT,  int_handler);     // handle Ctrl-C

    alarm(1);                         // first alarm in 1 second

    while (1) {
        if (handled) {
            printf("Turing was right!\n");
            handled = 0;
        }
    }

    return 0;  // never reached
}