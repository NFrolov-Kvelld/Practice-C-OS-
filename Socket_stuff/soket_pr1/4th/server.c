#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5000

int main() {
    int server_socket, client_socket;
    if ((server_socket=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Server started on port %d\n", PORT);
while (1) { //ЧТО ЗА ПЕРЕМЕННАЯ РЕГУЛИРУЕТ ЧИСЛО ПОДКЛЮЧЕННЫХ ???
    socklen_t client_len = sizeof(client_socket);
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_socket, &client_len)) == -1) {
        perror("accept");
        close(server_socket);
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Connection established\n");

    char buffer[255];
    ssize_t bytes_res = recv(client_socket,buffer,sizeof(buffer),0);
    printf("Message: %s\n",buffer);
    close(client_socket);
}
    close(server_socket);
    return EXIT_SUCCESS;
}