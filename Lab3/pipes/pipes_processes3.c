#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep <arg> | sort".
 * Takes one command line argument that is passed to grep.
 * Creates three processes:
 *   P1 (Parent)        - executes "cat scores"
 *   P2 (Child)         - executes "grep <arg>"
 *   P3 (Child's Child) - executes "sort"
 *
 * Two pipes are used:
 *   pipe1: connects P1 (cat) output to P2 (grep) input
 *   pipe2: connects P2 (grep) output to P3 (sort) input
 */

int main(int argc, char **argv)
{
    /* Check that a grep argument was provided */
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
        exit(1);
    }

    int pipe1[2];  /* pipe between P1 (cat) and P2 (grep) */
    int pipe2[2];  /* pipe between P2 (grep) and P3 (sort) */

    char *cat_args[]  = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    /* Create pipe1 - between cat and grep */
    if (pipe(pipe1) == -1)
    {
        fprintf(stderr, "Pipe 1 failed\n");
        exit(1);
    }

    /* Create pipe2 - between grep and sort */
    if (pipe(pipe2) == -1)
    {
        fprintf(stderr, "Pipe 2 failed\n");
        exit(1);
    }

    /* Fork P2 (grep process) */
    int pid1 = fork();

    if (pid1 < 0)
    {
        fprintf(stderr, "Fork 1 failed\n");
        exit(1);
    }

    if (pid1 == 0)
    {
        /* ── P2 (Child) - will handle grep ── */

        /* Fork P3 (sort process) */
        int pid2 = fork();

        if (pid2 < 0)
        {
            fprintf(stderr, "Fork 2 failed\n");
            exit(1);
        }

        if (pid2 == 0)
        {
            /* ── P3 (Child's Child) - handles sort ── */

            /* Replace stdin with read end of pipe2 */
            if (dup2(pipe2[0], 0) == -1)
            {
                fprintf(stderr, "dup2 failed in P3\n");
                exit(1);
            }

            /* Close all pipe ends - P3 does not need them */
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);

            /* Execute sort - receives grep output via pipe2 */
            if (execvp("sort", sort_args) == -1)
            {
                fprintf(stderr, "execvp sort failed\n");
                exit(1);
            }
        }
        else
        {
            /* ── P2 (Child) - handles grep ── */

            /* Replace stdin with read end of pipe1 (input from cat) */
            if (dup2(pipe1[0], 0) == -1)
            {
                fprintf(stderr, "dup2 stdin failed in P2\n");
                exit(1);
            }

            /* Replace stdout with write end of pipe2 (output to sort) */
            if (dup2(pipe2[1], 1) == -1)
            {
                fprintf(stderr, "dup2 stdout failed in P2\n");
                exit(1);
            }

            /* Close all pipe ends - P2 has duplicated what it needs */
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);

            /* Wait for P3 (sort) to finish */
            wait(NULL);

            /* Execute grep - receives cat output, sends to sort */
            if (execvp("grep", grep_args) == -1)
            {
                fprintf(stderr, "execvp grep failed\n");
                exit(1);
            }
        }
    }
    else
    {
        /* ── P1 (Parent) - handles cat ── */

        /* Replace stdout with write end of pipe1 (output to grep) */
        if (dup2(pipe1[1], 1) == -1)
        {
            fprintf(stderr, "dup2 stdout failed in P1\n");
            exit(1);
        }

        /* Close all pipe ends - P1 has duplicated what it needs */
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        /* Wait for P2 (grep) to finish */
        wait(NULL);

        /* Execute cat - sends output to grep via pipe1 */
        if (execvp("cat", cat_args) == -1)
        {
            fprintf(stderr, "execvp cat failed\n");
            exit(1);
        }
    }

    return 0;
}