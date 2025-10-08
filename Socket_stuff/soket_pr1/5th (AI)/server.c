#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 5000
#define BUFFER_SIZE 1024
#define BACKLOG 5

int main() {
    int tcp_socket, udp_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len;

    // 1. Создание TCP-сокета
    if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket (TCP)");
        exit(EXIT_FAILURE);
    }

    // 2. Создание UDP-сокета
    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket (UDP)");
        exit(EXIT_FAILURE);
    }

    // 3. Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 4. Привязка TCP-сокета
    if (bind(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind (TCP)");
        close(tcp_socket);
        close(udp_socket);
        exit(EXIT_FAILURE);
    }

    // 5. Привязка UDP-сокета
    if (bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind (UDP)");
        close(tcp_socket);
        close(udp_socket);
        exit(EXIT_FAILURE);
    }

    // 6. TCP: ожидание соединений
    if (listen(tcp_socket, BACKLOG) == -1) {
        perror("listen (TCP)");
        close(tcp_socket);
        close(udp_socket);
        exit(EXIT_FAILURE);
    }

    printf("Мультипротокольный сервер запущен на порту %d.\n", PORT);

    fd_set read_fds; // Набор файловых дескрипторов для чтения
    int max_fd = (tcp_socket > udp_socket) ? tcp_socket : udp_socket;

    while (1) {
        // Очистка и установка набора дескрипторов
        FD_ZERO(&read_fds);
        FD_SET(tcp_socket, &read_fds);
        FD_SET(udp_socket, &read_fds);

        // 7. Использование select()
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            close(tcp_socket);
            close(udp_socket);
            exit(EXIT_FAILURE);
        }

        // 8. Проверка, если TCP-сокет готов к чтению (т.е., есть новое соединение)
        if (FD_ISSET(tcp_socket, &read_fds)) {
            int new_client_socket;
            client_len = sizeof(client_addr);
            if ((new_client_socket = accept(tcp_socket, (struct sockaddr *)&client_addr, &client_len)) == -1) {
                perror("accept");
                continue;
            }
            printf("Новое TCP-соединение от %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            
            // Получаем сообщение от TCP-клиента
            ssize_t bytes_received = recv(new_client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                printf("Получено по TCP: %s\n", buffer);
            }
            close(new_client_socket); // Закрываем сокет клиента
        }

        // 9. Проверка, если UDP-сокет готов к чтению (т.е., есть новая датаграмма)
        if (FD_ISSET(udp_socket, &read_fds)) {
            client_len = sizeof(client_addr);
            ssize_t bytes_received = recvfrom(udp_socket, buffer, BUFFER_SIZE, 0,
                                              (struct sockaddr *)&client_addr, &client_len);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                printf("Получено по UDP от %s:%d: %s\n", inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port), buffer);
            }
        }
    }

    // Закрытие сокетов (недостижимо в бесконечном цикле)
    close(tcp_socket);
    close(udp_socket);
    return 0;
}