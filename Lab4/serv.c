#include <netinet/in.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include "list.h"

#define PORT 9001
#define ACK  "ACK "

/* Global variables for graceful shutdown */
int clientSocket;
int servSockD;
list_t *mylist;

/* Signal handler for Ctrl-C - gracefully frees memory and exits */
void sigint_handler(int sig)
{
    printf("\nServer shutting down gracefully...\n");
    list_free(mylist);
    close(clientSocket);
    close(servSockD);
    exit(0);
}

int main(int argc, char const* argv[]) 
{ 
    int n, val, idx;

    /* Register signal handler for Ctrl-C */
    signal(SIGINT, sigint_handler);

    /* Create server socket */
    servSockD = socket(AF_INET, SOCK_STREAM, 0); 
    if (servSockD < 0)
    {
        fprintf(stderr, "Socket creation failed\n");
        exit(1);
    }

    /* Allow address reuse to prevent "address already in use" errors */
    int opt = 1;
    setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Buffers for receiving and sending data */
    char buf[1024];
    char sbuf[1024];
    char *token;

    /* Define server address */
    struct sockaddr_in servAddr; 
    servAddr.sin_family      = AF_INET; 
    servAddr.sin_port        = htons(PORT); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 

    /* Bind socket to IP and port */
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        fprintf(stderr, "Bind failed\n");
        exit(1);
    }

    /* Listen for connections */
    listen(servSockD, 1); 
    printf("Server listening on port %d...\n", PORT);

    /* Accept a client connection */
    clientSocket = accept(servSockD, NULL, NULL);
    printf("Client connected!\n");

    /* Create the linked list */
    mylist = list_alloc();

    while (1)
    {
        /* Receive message from client */
        n = recv(clientSocket, buf, sizeof(buf), 0);

        /* Check for client disconnect or recv error */
        if (n == 0)
        {
            printf("Client disconnected.\n");
            break;
        }
        else if (n < 0)
        {
            fprintf(stderr, "recv error\n");
            break;
        }

        buf[n] = '\0';

        if (n > 0)
        {
            token = strtok(buf, " ");

            /* ── exit ── */
            if (strcmp(token, "exit") == 0)
            {
                list_free(mylist);
                close(clientSocket);
                close(servSockD);
                exit(0);
            }

            /* ── print ── */
            else if (strcmp(token, "print") == 0)
            {
                char *listStr = listToString(mylist);
                sprintf(sbuf, "%s", listStr);
                free(listStr);
            }

            /* ── get_length ── */
            else if (strcmp(token, "get_length") == 0)
            {
                val = list_length(mylist);
                sprintf(sbuf, "Length = %d", val);
            }

            /* ── add_front <value> ── */
            else if (strcmp(token, "add_front") == 0)
            {
                token = strtok(NULL, " ");
                if (token == NULL) { sprintf(sbuf, "Error: missing value"); }
                else {
                    val = atoi(token);
                    list_add_to_front(mylist, val);
                    sprintf(sbuf, "%s%d", ACK, val);
                }
            }

            /* ── add_back <value> ── */
            else if (strcmp(token, "add_back") == 0)
            {
                token = strtok(NULL, " ");
                if (token == NULL) { sprintf(sbuf, "Error: missing value"); }
                else {
                    val = atoi(token);
                    list_add_to_back(mylist, val);
                    sprintf(sbuf, "%s%d", ACK, val);
                }
            }

            /* ── add_position <index> <value> ── */
            else if (strcmp(token, "add_position") == 0)
            {
                token = strtok(NULL, " ");
                if (token == NULL) { sprintf(sbuf, "Error: missing index"); }
                else {
                    idx   = atoi(token);
                    token = strtok(NULL, " ");
                    if (token == NULL) { sprintf(sbuf, "Error: missing value"); }
                    else {
                        val = atoi(token);
                        list_add_at_index(mylist, val, idx);
                        sprintf(sbuf, "%s%d", ACK, val);
                    }
                }
            }

            /* ── remove_front ── */
            else if (strcmp(token, "remove_front") == 0)
            {
                val = list_remove_from_front(mylist);
                sprintf(sbuf, "%s%d", ACK, val);
            }

            /* ── remove_back ── */
            else if (strcmp(token, "remove_back") == 0)
            {
                val = list_remove_from_back(mylist);
                sprintf(sbuf, "%s%d", ACK, val);
            }

            /* ── remove_position <index> ── */
            else if (strcmp(token, "remove_position") == 0)
            {
                token = strtok(NULL, " ");
                if (token == NULL) { sprintf(sbuf, "Error: missing index"); }
                else {
                    idx = atoi(token);
                    val = list_remove_at_index(mylist, idx);
                    sprintf(sbuf, "%s%d", ACK, val);
                }
            }

            /* ── get <index> ── */
            else if (strcmp(token, "get") == 0)
            {
                token = strtok(NULL, " ");
                if (token == NULL) { sprintf(sbuf, "Error: missing index"); }
                else {
                    idx = atoi(token);
                    val = list_get_elem_at(mylist, idx);
                    sprintf(sbuf, "%s%d", ACK, val);
                }
            }

            /* ── unknown command ── */
            else
            {
                sprintf(sbuf, "Unknown command: %s", token);
            }

            /* Send response to client */
            send(clientSocket, sbuf, sizeof(sbuf), 0);
        }

        memset(buf,  '\0', 1024);
        memset(sbuf, '\0', 1024);
    }

    /* Clean up on disconnect */
    list_free(mylist);
    close(clientSocket);
    close(servSockD);
    return 0; 
}