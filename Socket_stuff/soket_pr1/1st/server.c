#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int server_socket;
    if ((server_socket = socket(AF_UNIX,SOCK_DGRAM,0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_un server;
    memset(&server,0,sizeof(server));
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, "/tmp/firstSocket");
    unlink("/tmp/firstSocket");
    if (bind(server_socket,(struct sockaddr*)&server,sizeof(server)) == -1){
        perror("bind");
        return EXIT_FAILURE;
    }
    printf("Server started\n");
    char buffer[255];
    struct sockaddr_un client;
    socklen_t client_len = sizeof(client);
    while (1)
    {
    ssize_t bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &client_len);
    printf("Received from client: %s\n", buffer);
    }
    close(server_socket);
    unlink("/tmp/firstSocket");
    return EXIT_SUCCESS;
}