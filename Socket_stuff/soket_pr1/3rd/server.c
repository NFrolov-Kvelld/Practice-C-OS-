#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define PATH "/tmp/secondSocket"

int main(){
    int server_socket, client_socket;
    if ((server_socket=socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, PATH);

    unlink(PATH);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("owerflow");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Server started\n");

    socklen_t client_len = sizeof(struct sockaddr_un);
    if ((client_socket = accept(server_socket,(struct sockaddr*)&client_socket, &client_len))==-1) {
        perror("accept");
        close(server_socket);
        exit(EXIT_FAILURE);
    };
    printf("Connection established\n"); // какие слова знает clion

    char buffer[255];
    ssize_t bytes_res = recv(client_socket,buffer,sizeof(buffer),0);
    printf("Message: %s\n",buffer);

    close(server_socket);
    close(client_socket);
    unlink(PATH);

    return EXIT_SUCCESS;
}