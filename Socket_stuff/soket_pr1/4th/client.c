#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVER_IP "127.0.0.5"
#define PORT 5050

int main() {
    int client_socket;
    if ((client_socket=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    if ((connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address))) == -1) {
        perror("connect");
        close(client_socket);
        return EXIT_FAILURE;
    }
    printf("Connected\n");


    char message[255];
    fgets(message, sizeof(message), stdin);
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
    close(client_socket);
    return EXIT_SUCCESS;
}