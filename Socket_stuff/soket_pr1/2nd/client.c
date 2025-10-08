#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVER_IP "127.0.0.5"
#define PORT 5000

int main() {
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr.s_addr);

    char message[255];
    fgets(message, sizeof(message), stdin);
    sendto(client_socket, message, strlen(message),0,(struct sockaddr*)&server, sizeof(server));

    close(client_socket);
    return EXIT_SUCCESS;
}