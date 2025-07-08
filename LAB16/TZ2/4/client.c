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
    
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    // Отправка запроса
    const char *message = "TIME";
    write(client_fd, message, strlen(message));
    
    // Получение ответа
    ssize_t recv_len = read(client_fd, buffer, sizeof(buffer) - 1);
    if (recv_len < 0) {
        perror("read failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    buffer[recv_len] = '\0';
    printf("Текущее время: %s\n", buffer);
    
    close(client_fd);
    return 0;
}