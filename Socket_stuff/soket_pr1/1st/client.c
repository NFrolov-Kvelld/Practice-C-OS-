#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>



int main() {
    int client_socket;
    if ((client_socket = socket(AF_UNIX,SOCK_DGRAM,0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_un client;
    memset(&client,0,sizeof(client)); //чтоб не было мусора
    client.sun_family = AF_UNIX;
    strcpy(client.sun_path, "/tmp/firstSocket");
while (1)
{
    char message[255];
    fgets(message, sizeof(message), stdin);
    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, "/tmp/firstSocket");
    sendto(client_socket, message, strlen(message), 0, (struct sockaddr*)&server, sizeof(server));
}
    close(client_socket);
    return EXIT_SUCCESS;
}