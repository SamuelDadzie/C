#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

/* Number of iterations for each process */
#define MAX_ITERATIONS 25

/* Index definitions for shared memory */
#define BANK_ACCOUNT 0
#define TURN         1

void ParentProcess(int *ShmPTR);
void ChildProcess(int *ShmPTR);

int main(int argc, char *argv[])
{
    int   ShmID;
    int   *ShmPTR;
    pid_t pid;
    int   status;

    /* Allocate shared memory for 2 integers: BankAccount and Turn */
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0)
    {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }

    /* Attach shared memory to address space */
    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1)
    {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }

    /* Initialize BankAccount and Turn to 0 */
    ShmPTR[BANK_ACCOUNT] = 0;
    ShmPTR[TURN]         = 0;

    /* Seed random number generator */
    srandom(getpid());

    /* Fork child process */
    pid = fork();
    if (pid < 0)
    {
        printf("*** fork error ***\n");
        exit(1);
    }
    else if (pid == 0)
    {
        /* Child process - Poor Student */
        ChildProcess(ShmPTR);
        exit(0);
    }
    else
    {
        /* Parent process - Dear Old Dad */
        ParentProcess(ShmPTR);
    }

    /* Parent waits for child to finish */
    wait(&status);

    /* Detach and remove shared memory */
    shmdt((void *) ShmPTR);
    shmctl(ShmID, IPC_RMID, NULL);

    exit(0);
}

/* Parent Process - Dear Old Dad */
void ParentProcess(int *ShmPTR)
{
    int account;
    int balance;
    int i;

    for (i = 0; i < MAX_ITERATIONS; i++)
    {
        /* Rule 1: Sleep random time between 0-5 seconds */
        sleep(random() % 6);

        /* Rule 2: Copy BankAccount to local variable */
        account = ShmPTR[BANK_ACCOUNT];

        /* Rule 3: Wait while Turn != 0 (strict alternation) */
        while (ShmPTR[TURN] != 0);

        /* Rule 4: Check account balance */
        if (account <= 100)
        {
            /* Try to deposit money */
            balance = (random() % 100) + 1;  /* random amount 1-100 */

            if (balance % 2 == 0)
            {
                /* Even: deposit */
                account += balance;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n",
                       balance, account);
            }
            else
            {
                /* Odd: no money to give */
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        }
        else
        {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n",
                   account);
        }

        /* Rule 4 & 5: Copy local account back to BankAccount, set Turn = 1 */
        ShmPTR[BANK_ACCOUNT] = account;
        ShmPTR[TURN]         = 1;
    }
}

/* Child Process - Poor Student */
void ChildProcess(int *ShmPTR)
{
    int account;
    int balance;
    int i;

    /* Seed differently from parent */
    srandom(getpid());

    for (i = 0; i < MAX_ITERATIONS; i++)
    {
        /* Rule 1: Sleep random time between 0-5 seconds */
        sleep(random() % 6);

        /* Rule 2: Copy BankAccount to local variable */
        account = ShmPTR[BANK_ACCOUNT];

        /* Rule 3: Wait while Turn != 1 (strict alternation) */
        while (ShmPTR[TURN] != 1);

        /* Rule 4: Randomly generate amount needed between 1-50 */
        balance = (random() % 50) + 1;
        printf("Poor Student needs $%d\n", balance);

        /* Rule 5 & 6: Check if enough cash */
        if (balance <= account)
        {
            /* Withdraw */
            account -= balance;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n",
                   balance, account);
        }
        else
        {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        /* Rule 7 & 8: Copy local account back to BankAccount, set Turn = 0 */
        ShmPTR[BANK_ACCOUNT] = account;
        ShmPTR[TURN]         = 0;
    }
}