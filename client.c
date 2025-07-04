#include <stdio.h>
#include <errno.h> 
#include <fcntl.h>  
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/ip.h>
#include <string.h>
void connection_handler(int sockFD);
int main()
{
    int socketfd, cstatus;
    struct sockaddr_in saddress;
    struct sockaddr server;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        perror("Error while creating s!");
        _exit(0);
    }
    saddress.sin_family = AF_INET;               
    saddress.sin_port = htons(8085);              
    saddress.sin_addr.s_addr = htonl(INADDR_ANY); 
    cstatus = connect(socketfd, (struct sockaddr *)&saddress, sizeof(saddress));
    if (cstatus == -1)
    {
        perror("Error while connecting to server!");
        close(socketfd);
        _exit(0);
    }
    connection_handler(socketfd);
    close(socketfd);
}

void connection_handler(int sockFD)
{
    char rbuffer[1000], wbuffer[1000],tempBuffer[1000]; 
    ssize_t rbytes, wbytes;            
    do
    {
        bzero(rbuffer, sizeof(rbuffer));
        bzero(tempBuffer, sizeof(tempBuffer));
        rbytes = read(sockFD, rbuffer, sizeof(rbuffer));
        if (rbytes == -1)
            perror("There is some error while reading!");
        else if (rbytes == 0)
            printf("Closing the connection to the server now!\n");
        else if (strchr(rbuffer,'^') != NULL)
        {
            // Skip read from client
            strncpy(tempBuffer, rbuffer, strlen(rbuffer) - 1);
            printf("%s\n", tempBuffer);
            wbytes = write(sockFD, "^", strlen("^"));
            if (wbytes == -1)
            {
                perror("Error while writing to client socket!");
                break;
            }
        }
        else if (strchr(rbuffer, '$') != NULL)
        {
            // Server sent an error message and is now closing it's end of the connection
            strncpy(tempBuffer, rbuffer, strlen(rbuffer) - 2);
            printf("%s\n", tempBuffer);
            printf("Closing the connection to the server now!\n");
            break;
        }
        else
        {
            bzero(wbuffer, sizeof(wbuffer)); // Empty the write buffer

            if (strchr(rbuffer, '#') != NULL)
                strcpy(wbuffer, getpass(rbuffer));
            else
            {
                printf("%s\n", rbuffer);
                scanf("%[^\n]%*c", wbuffer); // Take user input!
            }

            wbytes = write(sockFD, wbuffer, strlen(wbuffer));
            if (wbytes == -1)
            {
                perror("Error while writing to client socket!");
                printf("Closing the connection to the server now!\n");
                break;
            }
        }
    } while (rbytes > 0);
    close(sockFD);
}