/* signal2.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int handled = 0;  // flag toggled every time handler fires

void handler(int signum)
{
    printf("Hello World!\n");
    handled = 1;        // notify main
    alarm(5);           // reschedule for another 5 seconds
}

int main(int argc, char * argv[])
{
    signal(SIGALRM, handler);  // register handler for SIGALRM
    alarm(5);                  // first alarm in 5 seconds

    while (1) {                // loop forever
        if (handled) {
            printf("Turing was right!\n");
            handled = 0;       // reset flag, wait for next alarm
        }
    }

    return 0;  // never reached
}
