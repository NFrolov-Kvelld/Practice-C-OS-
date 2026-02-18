#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 0
#define BUFFER_SIZE 255

int main() {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_address);

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT); 

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return EXIT_FAILURE;
    }
    if (getsockname(server_socket, (struct sockaddr *)&server_address, &addr_len) == -1) {
        perror("getsockname failed");
        return EXIT_FAILURE;
    }

    printf("Port found and opened: %d\n", ntohs(server_address.sin_port));

    while (1) {
        socklen_t client_len = sizeof(client_address);
        ssize_t recv_bytes = recvfrom(server_socket, buffer, BUFFER_SIZE - 1, 0, 
                                      (struct sockaddr *)&client_address, &client_len);
        
        if (recv_bytes > 0) {
            buffer[recv_bytes] = '\0';
            printf("[%s:%d] Received: %s", inet_ntoa(client_address.sin_addr), 
                   ntohs(client_address.sin_port), buffer);

            char response[BUFFER_SIZE + 20];
            sprintf(response, "Server processed: %s", buffer);

            sendto(server_socket, response, strlen(response), 0, 
                   (struct sockaddr *)&client_address, client_len);
        }
    }

    close(server_socket);
    return EXIT_SUCCESS;
}