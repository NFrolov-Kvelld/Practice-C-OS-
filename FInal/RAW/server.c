#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUF_SIZE 1024
#define SERVER_PORT 5000

typedef struct {
    in_addr_t ip_addr;
    in_port_t port;
    int counter;
    int active;
} client_info;

client_info clients[MAX_CLIENTS];
int client_count = 0;

int clientStaff(in_addr_t ip, in_port_t port) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].ip_addr == ip && clients[i].port == port) {
            return i;
        }
    }
    if (client_count < MAX_CLIENTS) {
        clients[client_count].ip_addr = ip;
        clients[client_count].port = port;
        clients[client_count].counter = 0;
        clients[client_count].active = 1;
        return client_count++;
    }
    return -1;
}

//так и не понял как это делать. Взято с интернета
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

int main() {
    int sock;
    char buffer[BUF_SIZE];
    /*
        *Сокет построен на IPPROTO_UDP, так-как IPPROTO_RAW - вредничал (как и клиент соответственно)
        */
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int on = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
        perror("setsockopt failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Server has been launched on port:%d...\n", SERVER_PORT);

    while (1) {
        if (recv(sock, buffer, BUF_SIZE, 0) < 0) {
            perror("recv failed");
            continue;
        }

        struct iphdr *ip_header = (struct iphdr *)buffer;
        if (ip_header->ihl * 4 + sizeof(struct udphdr) > BUF_SIZE) {
            continue;
        }
        struct udphdr *udp_header = (struct udphdr *)(buffer + ip_header->ihl * 4);
        char *data = buffer + ip_header->ihl * 4 + sizeof(struct udphdr);

        if (ntohs(udp_header->dest) != SERVER_PORT) {
            continue;
        }

        in_addr_t sender_ip = ip_header->saddr;
        in_port_t sender_port = ntohs(udp_header->source);
        //начало магии сервера...
        int clientIndx = clientStaff(sender_ip, sender_port);
        if (clientIndx == -1) {
            printf("Client limit.\n");
            continue;
        }

        struct in_addr client_ip_addr;
        client_ip_addr.s_addr = sender_ip;

        if (strncmp(data, "exit", 4) == 0) {
            printf("Client %s:%d disconnected. Counter reset.\n",
                   inet_ntoa(client_ip_addr), sender_port);
            clients[clientIndx].counter = 0;
            continue;
        }

        clients[clientIndx].counter++;

        char responseDatagramm[BUF_SIZE];
        int data_len = ntohs(udp_header->len) - sizeof(struct udphdr);
        memcpy(responseDatagramm, data, data_len);
        responseDatagramm[data_len] = '\0';

        char final_response[BUF_SIZE];
        sprintf(final_response, "%s %d", responseDatagramm, clients[clientIndx].counter);

        printf("Received from %s:%d: '%s'. Sending back: '%s'\n",
               inet_ntoa(client_ip_addr), sender_port, responseDatagramm, final_response);

        char packet[BUF_SIZE];
        struct iphdr *resp_ip_header = (struct iphdr *)packet;
        struct udphdr *resp_udp_header = (struct udphdr *)(packet + sizeof(struct iphdr));
        char *resp_data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
        int resp_data_len = strlen(final_response);

        resp_ip_header->ihl = 5;
        resp_ip_header->version = 4;
        resp_ip_header->tos = 0;
        resp_ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + resp_data_len);
        resp_ip_header->id = htons(rand());
        resp_ip_header->frag_off = 0;
        resp_ip_header->ttl = 64;
        resp_ip_header->protocol = IPPROTO_UDP;
        resp_ip_header->check = 0;
        resp_ip_header->saddr = ip_header->daddr;
        resp_ip_header->daddr = ip_header->saddr;

        resp_udp_header->source = htons(SERVER_PORT);
        resp_udp_header->dest = htons(sender_port);
        resp_udp_header->len = htons(sizeof(struct udphdr) + resp_data_len);
        resp_udp_header->check = 0;

        memcpy(resp_data, final_response, resp_data_len);

        resp_ip_header->check = csum((unsigned short *)packet, resp_ip_header->ihl * 4);

        struct sockaddr_in reply_addr;
        reply_addr.sin_family = AF_INET;
        reply_addr.sin_addr.s_addr = ip_header->saddr;
        reply_addr.sin_port = 0;

        if (sendto(sock, packet, ntohs(resp_ip_header->tot_len), 0,
                   (struct sockaddr *)&reply_addr, sizeof(reply_addr)) < 0) {
            perror("sendto failed");
        }
    }

    close(sock);
    return 0;
}