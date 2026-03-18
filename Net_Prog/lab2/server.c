#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/signal.h>
#define BUFFER_SIZE 255

int main() {

    signal(SIGCHLD, SIG_IGN);

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t addr_len = sizeof(server_address);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(0);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    if (getsockname(server_socket, (struct sockaddr *)&server_address, &addr_len) == -1) {
        perror("getsockname failed");
        return EXIT_FAILURE;
    }
    printf("Server has started on port  %d\n", ntohs(server_address.sin_port));

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        return EXIT_FAILURE;
    }

    while (1) {
        socklen_t client_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            close(client_socket);
        } else if (pid == 0) {
            //ДП
            close(server_socket);
            char buffer[BUFFER_SIZE];
            while (1) {
                ssize_t n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
                if (n <= 0) break;
                buffer[n] = '\0';
                int val = atoi(buffer);
                pid_t pd= getpid();
                printf("[%s:%d] Received on pid: %d %s", inet_ntoa(client_address.sin_addr),
                  ntohs(client_address.sin_port), pd ,buffer);
                int result = val++;
                char response[BUFFER_SIZE];
                sprintf(response, "%d", result);

                send(client_socket, response, strlen(response), 0);
            }

            close(client_socket);
            printf("\n");
            exit(0);
        } else {
            //waitpid(pid, NULL, 0);
            close(client_socket);
        }
    }

    close(server_socket);
    return EXIT_SUCCESS;
}