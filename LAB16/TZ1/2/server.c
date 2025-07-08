#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <time.h>

#define SOCKET_PATH "/tmp/unix_socket_stream"
#define MAX_CLIENTS 5

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH);
    
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
    
    printf("Сервер UNIX STREAM запущен. Ожидание подключений...\n");
    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
            perror("accept failed");
            continue;
        }
        
        ssize_t recv_len = read(client_fd, buffer, sizeof(buffer));
        if (recv_len < 0) {
            perror("read failed");
            close(client_fd);
            continue;
        }
        
        buffer[recv_len] = '\0';
        printf("Получено: %s\n", buffer);

        time_t raw_time;
        struct tm *time_info;
        char time_str[100];
        
        time(&raw_time);
        time_info = localtime(&raw_time);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        
        write(client_fd, time_str, strlen(time_str));
        close(client_fd);
    }
    
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}