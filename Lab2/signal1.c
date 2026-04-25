/* signal1.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int handled = 0;  // global variable to track if signal was handled

void handler(int signum)
{
    printf("Hello World!\n");
    handled = 1;  // set flag so main() knows handler was called
}

int main(int argc, char * argv[])
{
    signal(SIGALRM, handler);  // register handler for SIGALRM
    alarm(5);                  // schedule SIGALRM in 5 seconds

    while (!handled);          // busy wait until handler sets the flag

    printf("Turing was right!\n");  // print after handler returns
    return 0;
}