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
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address, client_address;

    memset(&client_address, 0, sizeof(client_address));
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons(PORT);

    if (bind(client_socket, (struct sockaddr *) &client_address, sizeof(client_address)) == -1) {
        perror("bind");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    inet_pton(AF_INET, MULTICAST_GROUP, &mreq.imr_multiaddr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(client_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1) {
        perror("setsockopt");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Subscribed\n");

    char buffer[255];
    socklen_t slen = sizeof(server_address);
    ssize_t received = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &server_address, &slen);
    printf("Message: %s\n",buffer);

    close(client_socket);
    return EXIT_SUCCESS;
}