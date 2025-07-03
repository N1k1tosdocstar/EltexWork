#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100
#define EXIT_CMD "EXIT"
#define RESPONSE_SIZE (BUFFER_SIZE + 20) // Увеличиваем буфер для ответа

typedef struct {
    struct in_addr ip;
    in_port_t port;
    int count;
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];
int client_count = 0;

ClientInfo* find_client(struct sockaddr_in *addr) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].ip.s_addr == addr->sin_addr.s_addr && 
            clients[i].port == addr->sin_port) {
            return &clients[i];
        }
    }
    return NULL;
}

void remove_client(struct sockaddr_in *addr) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].ip.s_addr == addr->sin_addr.s_addr && 
            clients[i].port == addr->sin_port) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j+1];
            }
            client_count--;
            printf("Client %s:%d removed\n", 
                   inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
            return;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Добавляем SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d. Waiting for messages...\n", PORT);

    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, 
                            (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';

        if (strcmp(buffer, EXIT_CMD) == 0) {
            remove_client(&cliaddr);
            printf("Client %s:%d exited\n", 
                   inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            continue;
        }

        ClientInfo *client = find_client(&cliaddr);
        if (!client) {
            if (client_count >= MAX_CLIENTS) {
                fprintf(stderr, "Max clients reached\n");
                continue;
            }
            clients[client_count].ip = cliaddr.sin_addr;
            clients[client_count].port = cliaddr.sin_port;
            clients[client_count].count = 0;
            client = &clients[client_count];
            client_count++;
            printf("New client: %s:%d\n", 
                   inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        }

        client->count++;
        char response[RESPONSE_SIZE];
        snprintf(response, sizeof(response), "%s %d", buffer, client->count);
        
        sendto(sockfd, response, strlen(response), 0, 
              (struct sockaddr *)&cliaddr, len);
        
        printf("Sent to %s:%d: %s\n", inet_ntoa(cliaddr.sin_addr), 
               ntohs(cliaddr.sin_port), response);
    }

    close(sockfd);
    return 0;
}