// C program to demonstrate use of fork() and pipe()
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>

int main()
{
    // We use two pipes
    // First pipe to send input string from parent (P1) to child (P2)
    // Second pipe to send concatenated string back from child (P2) to parent (P1)

    int fd1[2];  // Two ends of first pipe  (P1 -> P2)
    int fd2[2];  // Two ends of second pipe (P2 -> P1)

    char fixed_str[]  = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[100];
    pid_t p;

    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    // P1 prompts user for first input
    printf("Input : ");
    scanf("%s", input_str);

    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }

    // Parent process (P1)
    else if (p > 0)
    {
        close(fd1[0]);  // Close reading end of first pipe
        close(fd2[1]);  // Close writing end of second pipe

        // Send input string to child via first pipe
        if (write(fd1[1], input_str, strlen(input_str) + 1) == -1)
        {
            fprintf(stderr, "Write to fd1 failed");
            return 1;
        }
        close(fd1[1]);  // Done writing to first pipe

        // Wait for child to finish
        wait(NULL);

        // Read the string sent back from child via second pipe
        char recv_str[200];
        if (read(fd2[0], recv_str, 200) == -1)
        {
            fprintf(stderr, "Read from fd2 failed");
            return 1;
        }
        close(fd2[0]);

        // Append "gobison.org" and print final result
        int k = strlen(recv_str);
        int i;
        for (i = 0; i < strlen(fixed_str2); i++)
            recv_str[k++] = fixed_str2[i];
        recv_str[k] = '\0';

        printf("Output : %s\n", recv_str);
    }

    // Child process (P2)
    else
    {
        close(fd1[1]);  // Close writing end of first pipe
        close(fd2[0]);  // Close reading end of second pipe

        // Read the string sent from parent via first pipe
        char concat_str[200];
        if (read(fd1[0], concat_str, 200) == -1)
        {
            fprintf(stderr, "Read from fd1 failed");
            return 1;
        }
        close(fd1[0]);

        // Append "howard.edu" and print
        int k = strlen(concat_str);
        int i;
        for (i = 0; i < strlen(fixed_str); i++)
            concat_str[k++] = fixed_str[i];
        concat_str[k] = '\0';

        printf("Output : %s\n", concat_str);
        fflush(stdout);

        // Prompt user for second input
        printf("Input : ");
        fflush(stdout);
        char input_str2[100];
        scanf("%s", input_str2);

        // Append second input to the concatenated string
        k = strlen(concat_str);
        for (i = 0; i < strlen(input_str2); i++)
            concat_str[k++] = input_str2[i];
        concat_str[k] = '\0';

        // Send result back to parent via second pipe
        if (write(fd2[1], concat_str, strlen(concat_str) + 1) == -1)
        {
            fprintf(stderr, "Write to fd2 failed");
            return 1;
        }
        close(fd2[1]);

        exit(0);
    }
}