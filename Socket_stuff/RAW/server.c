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
    int count = 0;
    int server_socket;
    char buffer[4096];
    server_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (server_socket == -1) {
        perror("socket");
        close(server_socket);
        return EXIT_FAILURE;
    }
    printf("server is listening\n");
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    while (1)
        {
        socklen_t len = sizeof(address);
        ssize_t message_size = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr_in *) &address, &len);

        if (message_size == -1) {
            perror("recv");
            close(server_socket);
            return EXIT_FAILURE;
        }
        struct iphdr *ip = (struct iphdr *)buffer;
        if (ip->protocol != IPPROTO_UDP) {
            continue;
        }


        struct udphdr *udp = (struct udphdr *)(buffer + ip->ihl*4);
        if (ntohs(udp->dest) != MYPORTTO) {
            continue;
        }
        char *data = (char *)buffer + ip->ihl*4 + sizeof(struct udphdr);
        int data_len = ntohs(udp->len) - sizeof(struct udphdr);
        data[data_len] = '\0';

        printf("Server: Received message from %s:%d - %s\n",inet_ntoa(address.sin_addr), ntohs(udp->source), data);

        char response[256];
        snprintf(response, sizeof(response), "TAKE IT BACK!!!!: %s", data);
        ////////////////////////////
        char datagram[4096];
        struct iphdr *res_ip = (struct iphdr *)datagram;
        struct udphdr *res_udp = (struct udphdr *)(datagram + sizeof(struct iphdr));

        struct sockaddr_in send_addr;
        memset(&send_addr, 0, sizeof(send_addr));
        send_addr.sin_family = AF_INET;
        send_addr.sin_addr.s_addr = ip->saddr;
        send_addr.sin_port = udp->source;

        res_ip->ihl = 5;
        res_ip->version = 4;
        res_ip->tos = 0;
        res_ip->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(response);
        res_ip->id = htons(2+count);
        res_ip->frag_off = 0;
        res_ip->ttl = 255;
        res_ip->protocol = IPPROTO_UDP;
        res_ip->check = 0;
        res_ip->saddr = udp->dest;
        res_ip->daddr = udp->source;

        res_udp->source = udp->dest;
        res_udp->dest = udp->source;
        res_udp->len = htons(sizeof(struct udphdr) + strlen(response));
        res_udp->check = 0;

        strcpy(datagram + sizeof(struct iphdr) + sizeof(struct udphdr), response);

        res_ip->check = csum((unsigned short *)datagram, res_ip->ihl * 4);

        if (sendto(server_socket,datagram,res_ip->tot_len,0,(struct sockaddr *)&send_addr,sizeof(send_addr)) == -1) {
            perror("sendto");
            close(server_socket);
            return EXIT_FAILURE;
        }
    }
    close(server_socket);
    return EXIT_SUCCESS;
}