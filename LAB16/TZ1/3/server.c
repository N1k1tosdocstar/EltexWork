#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Сервер UDP запущен на порту %d\n", PORT);
    while (1) {
        ssize_t recv_len = recvfrom(server_fd, buffer, sizeof(buffer), 0,
                                    (struct sockaddr*)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        buffer[recv_len] = '\0';
        printf("Получено от %s:%d: %s\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
        
        time_t raw_time;
        struct tm *time_info;
        char time_str[100];
        
        time(&raw_time);
        time_info = localtime(&raw_time);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        
        sendto(server_fd, time_str, strlen(time_str), 0,
               (struct sockaddr*)&client_addr, client_len);
    }
    
    close(server_fd);
    return 0;
}