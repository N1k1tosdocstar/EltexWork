#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_CLIENTS 5

void handle_client(int client_fd) {
    char buffer[1024];
    
    // Чтение запроса
    ssize_t recv_len = read(client_fd, buffer, sizeof(buffer));
    if (recv_len < 0) {
        perror("read failed");
        close(client_fd);
        return;
    }
    
    buffer[recv_len] = '\0';
    printf("Получено: %s\n", buffer);
    
    // Отправка времени
    time_t raw_time;
    struct tm *time_info;
    char time_str[100];
    
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
    
    write(client_fd, time_str, strlen(time_str));
    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Параллельный TCP сервер запущен на порту %d\n", PORT);
    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
            perror("accept failed");
            continue;
        }
        
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            close(client_fd);
        } else if (pid == 0) { // Дочерний процесс
            close(server_fd);
            handle_client(client_fd);
            exit(EXIT_SUCCESS);
        } else { // Родительский процесс
            close(client_fd);
            waitpid(-1, NULL, WNOHANG); // Очистка зомби-процессов
        }
    }
    
    close(server_fd);
    return 0;
}