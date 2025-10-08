#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MYPORTTO 5000
#define AMIGOPORRTO 5001
#define MYIP "127.0.0.1"
unsigned short csum(unsigned short *ptr, int nbytes) {
    long sum;
    unsigned short oddbyte;
    short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;
    return answer;
}

int main() {
    int client_socket;
    client_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (client_socket == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    // ИСПРАВЛЕНИЕ 1: setsockopt
    // Нужно передавать указатель на переменную, а не просто значение.
    //!!!!!!!!!!!!!!!
    //!!!!!!!!!!!!!!!
    //А почему просто нельзя было передать 1 и sizeof(int)? Это вроде просто флаг...
    //!!!!!!!!!!!!!!!
    //!!!!!!!!!!!!!!!

    int optval = 1;
    if (setsockopt(client_socket,IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval))<0) {
        perror("setsockopt");
        close(client_socket);
        return EXIT_FAILURE;
    }

    char message[255];
    printf("Enter a message to send: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(MYIP);
    client_address.sin_port = htons(MYPORTTO);

    char datagram[4096];
    memset(&datagram, 0, sizeof(datagram));

    struct iphdr *ip = (struct iphdr *)datagram;
    struct udphdr *udp = (struct udphdr *)(datagram + sizeof(struct iphdr));

    ip->version = 4;
    ip->protocol = IPPROTO_UDP;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(message) ;
    ip->id = htons(1);
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->check = 0;
    ip->saddr = inet_addr(MYIP);
    ip->daddr = inet_addr(MYIP);

    // ИСПРАВЛЕНИЕ 2: Поменяли местами порты
    udp->source = htons(AMIGOPORRTO);
    udp->dest = htons(MYPORTTO);
    udp->len = htons(sizeof(struct udphdr) + strlen(message));
    udp->check = 0;

    strcpy(datagram + sizeof(struct iphdr)+sizeof(struct udphdr), message);
    ip->check = csum((unsigned short *)datagram, ip->ihl*4);
    if (sendto(client_socket, datagram, ip->tot_len, 0, (struct sockaddr *)&client_address, sizeof(client_address)) < 0) {
        perror("sendto");
        close(client_socket);
        return EXIT_FAILURE;
    }

    printf("Sending message...\n");
    close(client_socket);

    return EXIT_SUCCESS;
}