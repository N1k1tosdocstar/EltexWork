#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

void transform_message(char *msg) {
    // Пример преобразования: инвертирование регистра
    for (int i = 0; msg[i]; i++) {
        if (islower(msg[i])) 
            msg[i] = toupper(msg[i]);
        else if (isupper(msg[i]))
            msg[i] = tolower(msg[i]);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("UDP сервер запущен на порту %d\n", PORT);
    while (1) {
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr*)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        buffer[recv_len] = '\0';
        printf("Получено: %s\n", buffer);
        
        // Изменение сообщения
        transform_message(buffer);
        
        // Отправка измененного сообщения
        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr*)&client_addr, client_len);
    }
    
    close(sockfd);
    return 0;
}