#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("ENTER DATA IN CORRECT WAY!!\n: %s <IP> <Port> <I numburmbumumuuuu>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *serverIP = argv[1];
    int PORT = atoi(argv[2]);
    int INum = atoi(argv[3]);

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[255];

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket failed");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, serverIP, &server_addr.sin_addr);

    printf("send number %d, (%d) times with %d sec delay:\n", INum, INum, INum);

    for (int count = 0; count < INum; count++) {
        char message[50];
        sprintf(message, "%d (Message is #%d)\n", INum
    , count);

        sendto(client_socket, message, strlen(message), 0, 
               (struct sockaddr *)&server_addr, sizeof(server_addr));

        socklen_t addr_len = sizeof(server_addr);
        ssize_t recv_bytes = recvfrom(client_socket, buffer, sizeof(buffer) - 1, 0, 
                                      (struct sockaddr *)&server_addr, &addr_len);
        
        if (recv_bytes > 0) {
            buffer[recv_bytes] = '\0';
            printf("Take it - %d: %s", count, buffer);
        }

        sleep(INum);
    }

    close(client_socket);
    return EXIT_SUCCESS;
}