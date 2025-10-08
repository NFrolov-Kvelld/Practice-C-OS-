#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MULTICAST_GROUP "239.0.0.0"
#define PORT 5000

int main() {
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, MULTICAST_GROUP, &server_address.sin_addr);

    printf("Sending message...\n");

    char message[255];
    fgets(message, sizeof(message), stdin);
    sendto(server_socket, message, strlen(message), 0, (struct sockaddr*)&server_address, sizeof(server_address));

    close(server_socket);
    return EXIT_SUCCESS;
}