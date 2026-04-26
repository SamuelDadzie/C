// main3.c
//
// Creates two child processes. Each child loops a random number
// of times (up to 30), sleeping a random amount (up to 10 sec)
// each iteration. The parent waits for both children to finish.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void child_process() {
    // Seed with pid so each child gets different random numbers
    srandom(getpid());

    int iterations = (random() % 30) + 1;  // 1 to 30

    for (int i = 0; i < iterations; i++) {
        printf("Child Pid: %d is going to sleep!\n", getpid());
        fflush(stdout);

        int sleep_time = (random() % 10) + 1;  // 1 to 10 seconds
        sleep(sleep_time);

        printf("Child Pid: %d is awake!\nWhere is my Parent: %d?\n",
               getpid(), getppid());
        fflush(stdout);
    }

    exit(0);
}

int main() {
    pid_t pid1, pid2;
    int status;

    // Fork first child
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork() failed for child 1");
        exit(1);
    } else if (pid1 == 0) {
        child_process();
    }

    // Fork second child
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork() failed for child 2");
        exit(1);
    } else if (pid2 == 0) {
        child_process();
    }

    // Parent: wait for both children
    pid_t finished;
    while ((finished = wait(&status)) > 0) {
        printf("Child Pid: %d has completed\n", finished);
        fflush(stdout);
    }

    return 0;
}