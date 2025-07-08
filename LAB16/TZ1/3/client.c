#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    
    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    const char *message = "TIME";
    sendto(client_fd, message, strlen(message), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // Получение ответа
    socklen_t server_len = sizeof(server_addr);
    ssize_t recv_len = recvfrom(client_fd, buffer, sizeof(buffer) - 1, 0,
                                (struct sockaddr*)&server_addr, &server_len);
    if (recv_len < 0) {
        perror("recvfrom failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    buffer[recv_len] = '\0';
    printf("Текущее время: %s\n", buffer);
    
    close(client_fd);
    return 0;
}