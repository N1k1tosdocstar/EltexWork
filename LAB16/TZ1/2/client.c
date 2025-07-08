#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_stream"

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[1024];
    
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
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