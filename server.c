#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <pthread.h>

#include "./functions/constants.h"
#include "./functions/admin.h"
#include "./functions/faculty_op.h"
#include "./functions/student_op.h"

void* connection_handler(void* connFD_ptr);

int main() {
    int socketfd, sbstatus, slstatus, connectfd;
    struct sockaddr_in serverAddress, clientAddress;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        perror("There is an error while creating a socket!");
        _exit(0);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8085);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    sbstatus = bind(socketfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (sbstatus == -1) {
        perror("There is an error while binding to a socket!");
        _exit(0);
    }

    slstatus = listen(socketfd, 10);
    if (slstatus == -1) {
        perror("There is an error while listening!");
        close(socketfd);
        _exit(0);
    }

    printf("🎉 Server is up and listening on port 8085\n");

    while (1) {
        socklen_t clsize = sizeof(clientAddress);
        connectfd = accept(socketfd, (struct sockaddr *)&clientAddress, &clsize);
        if (connectfd == -1) {
            perror("Error while accepting client connection!");
            continue;
        }

        int* connFD_ptr = malloc(sizeof(int));
        *connFD_ptr = connectfd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, connection_handler, connFD_ptr) != 0) {
            perror("Error creating thread");
            close(connectfd);
            free(connFD_ptr);
        } else {
            pthread_detach(tid);  // auto-clean up after thread exits
        }
    }

    close(socketfd);
    return 0;
}
void* connection_handler(void* connFD_ptr) {
    int connectfd = *((int*)connFD_ptr);
    free(connFD_ptr);

    printf("Client connected to the server! [FD: %d]\n", connectfd);

    char readBuffer[1000], writeBuffer[1000];
    ssize_t rbytes, wbytes;
    int choice;

    wbytes = write(connectfd, INITIAL_PROMPT, strlen(INITIAL_PROMPT));
    if (wbytes == -1)
        perror("Error while sending first prompt to the user!");
    else {
        bzero(readBuffer, sizeof(readBuffer));
        rbytes = read(connectfd, readBuffer, sizeof(readBuffer));
        if (rbytes == -1)
            perror("Error while reading from client");
        else if (rbytes == 0)
            printf("No data was sent by the client\n");
        else {
            choice = atoi(readBuffer);
            switch (choice) {
                case 1:
                    admin_operation_handler(connectfd);
                    break;
                case 2:
                    faculty_operation_handler(connectfd);
                    break;
                case 3:
                    student_operation_handler(connectfd);
                    break;
                case 4:
                    break;
            }
        }
    }

    printf("Closing connection with client [FD: %d]\n", connectfd);
    close(connectfd);
    pthread_exit(NULL);
}
