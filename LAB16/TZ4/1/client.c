#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

// Кастомный заголовок UDP
struct custom_udp_header {
    u_int16_t source_port;
    u_int16_t dest_port;
    u_int16_t length;
    u_int16_t checksum;
};

unsigned short checksum(void *b, int len) {
    u_short *buf = b;
    u_int sum = 0;
    for (sum = 0; len > 1; len -= 2) 
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024], packet[1024];
    struct custom_udp_header *udp_hdr = (struct custom_udp_header*)packet;
    
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    // Заполнение UDP заголовка
    udp_hdr->source_port = htons(1234); // Произвольный порт
    udp_hdr->dest_port = htons(PORT);
    udp_hdr->length = htons(sizeof(struct custom_udp_header) + 5); // 5 = "HELLO"
    udp_hdr->checksum = 0;
    
    // Данные
    char *data = packet + sizeof(struct custom_udp_header);
    strcpy(data, "HELLO");
    
    // Расчёт контрольной суммы
    udp_hdr->checksum = checksum(udp_hdr, sizeof(struct custom_udp_header) + 5);
    
    // Отправка пакета
    if (sendto(sockfd, packet, sizeof(struct custom_udp_header) + 5, 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("Пакет отправлен\n");
    close(sockfd);
    return 0;
}