#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
  
#define PORT 9001
#define MAX_COMMAND_LINE_LEN 1024

char* getCommandLine(char *command_line){
    do {
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
            fprintf(stderr, "fgets error");
            exit(0);
        }
    } while(command_line[0] == 0x0A);  /* while just ENTER pressed */
    command_line[strlen(command_line) - 1] = '\0';
    return command_line;
}

int main(int argc, char const* argv[]) 
{ 
    int sockID = socket(AF_INET, SOCK_STREAM, 0); 
    char  *token, *cp;
    char buf[MAX_COMMAND_LINE_LEN];
    char responeData[MAX_COMMAND_LINE_LEN];

    struct sockaddr_in servAddr; 
    servAddr.sin_family      = AF_INET; 
    servAddr.sin_port        = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY; 
  
    int connectStatus = connect(sockID, (struct sockaddr*)&servAddr, sizeof(servAddr)); 
  
    if (connectStatus == -1) { 
        printf("Error...\n"); 
    } 
    else { 
        while(1) {
            printf("Enter Command (or menu): ");
            getCommandLine(buf);

            send(sockID, buf, strlen(buf), 0);

            cp    = buf;
            token = strtok(cp, " ");

            if (strcmp(token, "exit") == 0) {
                close(sockID);   /* gracefully close socket before exit */
                exit(0);
            }
            else if (strcmp(token, "menu") == 0) {
                printf("COMMANDS:\n---------\n"
                       "1. print\n"
                       "2. get_length\n"
                       "3. add_back <value>\n"
                       "4. add_front <value>\n"
                       "5. add_position <index> <value>\n"
                       "6. remove_back\n"
                       "7. remove_front\n"
                       "8. remove_position <index>\n"
                       "9. get <index>\n"
                       "10. exit\n");
            }

            recv(sockID, responeData, sizeof(responeData), 0);
            printf("\nSERVER RESPONSE: %s\n", responeData); 
            memset(buf, '\0', MAX_COMMAND_LINE_LEN);
        }
    } 
 
    return 0; 
}