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
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }
    struct sockaddr_in server_address, client_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if ((bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))) == -1) {
        perror("bind");
        close(server_socket);
        return EXIT_FAILURE;
    }
    printf("Server listening on port %d\n", PORT);
    char buffer[255];
    socklen_t client_len = sizeof(client_address);
    ssize_t recv_bytes = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_address, &client_len);
    printf("Received from client: IP = %s, Port = %d\n ", inet_ntoa(client_address.sin_addr),
           ntohs(client_address.sin_port));
    printf("Message: %s\n", buffer);
    close(server_socket);
    return EXIT_SUCCESS;
}
