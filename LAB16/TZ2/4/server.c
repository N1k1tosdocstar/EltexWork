#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>

#define TCP_PORT 8080
#define UDP_PORT 9090
#define MAX_CLIENTS 10

void handle_tcp_client(int client_fd) {
    char buffer[1024];
    ssize_t recv_len = recv(client_fd, buffer, sizeof(buffer), 0);
    if (recv_len <= 0) {
        close(client_fd);
        return;
    }
    
    buffer[recv_len] = '\0';
    
    time_t raw_time;
    struct tm *time_info;
    char time_str[100];
    
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
    
    send(client_fd, time_str, strlen(time_str), 0);
    close(client_fd);
}

void handle_udp_request(int udp_fd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];
    
    ssize_t recv_len = recvfrom(udp_fd, buffer, sizeof(buffer), 0,
                                (struct sockaddr*)&client_addr, &addr_len);
    if (recv_len <= 0) return;
    
    buffer[recv_len] = '\0';
    
    time_t raw_time;
    struct tm *time_info;
    char time_str[100];
    
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
    
    sendto(udp_fd, time_str, strlen(time_str), 0,
           (struct sockaddr*)&client_addr, addr_len);
}

int main() {
    int tcp_fd, udp_fd;
    struct sockaddr_in tcp_addr, udp_addr;
    fd_set read_fds;
    int max_fd;
    
    // TCP сокет
    if ((tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("TCP socket failed");
        exit(EXIT_FAILURE);
    }
    
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(TCP_PORT);
    
    if (bind(tcp_fd, (struct sockaddr*)&tcp_addr, sizeof(tcp_addr)) < 0) {
        perror("TCP bind failed");
        close(tcp_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(tcp_fd, MAX_CLIENTS) < 0) {
        perror("TCP listen failed");
        close(tcp_fd);
        exit(EXIT_FAILURE);
    }
    
    // UDP сокет
    if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP socket failed");
        exit(EXIT_FAILURE);
    }
    
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(UDP_PORT);
    
    if (bind(udp_fd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
        perror("UDP bind failed");
        close(udp_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Мультипротокольный сервер запущен:\n");
    printf("  TCP порт: %d\n", TCP_PORT);
    printf("  UDP порт: %d\n", UDP_PORT);
    
    max_fd = (tcp_fd > udp_fd) ? tcp_fd : udp_fd;
    
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(tcp_fd, &read_fds);
        FD_SET(udp_fd, &read_fds);
        
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select failed");
            continue;
        }
        
        if (FD_ISSET(tcp_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_fd = accept(tcp_fd, (struct sockaddr*)&client_addr, &addr_len);
            
            if (client_fd < 0) {
                perror("accept failed");
            } else {
                handle_tcp_client(client_fd);
            }
        }
        
        // Обработка UDP запросов
        if (FD_ISSET(udp_fd, &read_fds)) {
            handle_udp_request(udp_fd);
        }
    }
    
    close(tcp_fd);
    close(udp_fd);
    return 0;
}