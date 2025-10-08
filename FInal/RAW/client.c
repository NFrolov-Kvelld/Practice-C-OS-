#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define BUF_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000

int global_sockfd;
struct sockaddr_in global_server_addr;
int global_port;

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
        *((u_char*)&oddbyte) = *(u_char*)ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return answer;
}

//то-же, но с сигналом
void handle_signal(int sig) {
    printf("\nInterrupted. Sending 'exit' to server...\n");

    char packet[BUF_SIZE];
    char* message = "exit";
    int data_len = strlen(message);

    struct iphdr *ip = (struct iphdr *)packet;
    struct udphdr *udp = (struct udphdr *)(packet + sizeof(struct iphdr));
    char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);

    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    ip->id = htons(rand());
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    ip->saddr = inet_addr(SERVER_IP);
    ip->daddr = inet_addr(SERVER_IP);

    udp->len = htons(sizeof(struct udphdr) + data_len);
    udp->source = htons(global_port);
    udp->dest = htons(SERVER_PORT);

    memcpy(data, message, data_len);
    ip->check = csum((unsigned short *)packet, ip->ihl * 4);

    if (sendto(global_sockfd, packet, ntohs(ip->tot_len), 0,
               (struct sockaddr *)&global_server_addr, sizeof(global_server_addr)) < 0) {
        perror("sendto failed in signal handler");
    }

    close(global_sockfd);
    exit(0);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("socket");
        close(sockfd);
        return EXIT_FAILURE;
    }
    int flag = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) == -1) {
        perror("setsockopt(IP_HDRINCL)");
        close(sockfd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_adress;
    memset(&server_adress, 0, sizeof(server_adress));
    server_adress.sin_family = AF_INET;
    server_adress.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_adress.sin_port = htons(SERVER_PORT);

    int PORT;
    printf("enter port number: ");
    scanf("%d", &PORT);

    global_sockfd = sockfd;
    global_server_addr = server_adress;
    global_port = PORT;

    signal(SIGINT, handle_signal);

    while(getchar() != '\n' && getchar() != EOF);

    printf("connecting to server...\n");
    srand(time(0));

    while (1) {
        char message[BUF_SIZE];
        printf("Enter message: ");
        if (fgets(message, BUF_SIZE, stdin) == NULL) {
            break;
        }

        message[strcspn(message, "\n")] = 0;
        int data_len = strlen(message);

        char datagram[BUF_SIZE];
        memset(datagram, 0, BUF_SIZE);

        struct iphdr *ip = (struct iphdr *)datagram;
        struct udphdr *udp = (struct udphdr *)(datagram + sizeof(struct iphdr));
        char *data = (char *)(datagram + sizeof(struct iphdr) + sizeof(struct udphdr));

        ip->ihl = 5;
        ip->version = 4;
        ip->tos = 0;
        ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
        ip->id = htons(rand());
        ip->frag_off = 0;
        ip->ttl = 255;
        ip->protocol = IPPROTO_UDP;
        ip->check = 0;
        ip->saddr = inet_addr(SERVER_IP);
        ip->daddr = inet_addr(SERVER_IP);

        udp->len = htons(sizeof(struct udphdr) + data_len);
        udp->source = htons(PORT);
        udp->dest = htons(SERVER_PORT);

        memcpy(data, message, data_len);

        ip->check = csum((unsigned short *)datagram, ip->ihl * 4);

        if (strcmp(message, "exit") == 0) {
            if (sendto(sockfd, datagram, ntohs(ip->tot_len), 0,(struct sockaddr *)&server_adress, sizeof(server_adress)) < 0) {
                perror("sendto failed");
            }
            break;
        }

        if (sendto(sockfd, datagram, ntohs(ip->tot_len), 0,(struct sockaddr *)&server_adress, sizeof(server_adress)) < 0) {
            perror("sendto failed");
            continue;
        }

        while (1) {
            char recDatagram[BUF_SIZE];
            struct sockaddr_in recAdress;
            socklen_t addr_len = sizeof(recAdress);

            if (recvfrom(sockfd, recDatagram, BUF_SIZE, 0, (struct sockaddr *)&recAdress, &addr_len) < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                } else {
                    perror("recvfrom failed");
                    break;
                }
            }

            struct iphdr *rec_ip = (struct iphdr *)recDatagram;
            if (rec_ip->protocol != IPPROTO_UDP) {
                continue;
            }

            struct udphdr *rec_udp = (struct udphdr *)(recDatagram + rec_ip->ihl * 4);

            if (ntohs(rec_udp->source) == SERVER_PORT &&
                ntohs(rec_udp->dest) == PORT) {
                char *resData = (char *)(recDatagram + rec_ip->ihl * 4 + sizeof(struct udphdr));
                int resLen = ntohs(rec_udp->len) - sizeof(struct udphdr);
                char final_resp[resLen + 1];
                memcpy(final_resp, resData, resLen);
                final_resp[resLen] = '\0';
                printf("Server response: %s\n", final_resp);
                break;
                }
        }
    }

    printf("exiting...\n");
    close(sockfd);
    return EXIT_SUCCESS;
}