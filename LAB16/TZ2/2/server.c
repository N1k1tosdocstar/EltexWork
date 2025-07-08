#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define THREAD_POOL_SIZE 4
#define QUEUE_SIZE 20

typedef struct {
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    char buffer[1024];
    int len;
} Task;

Task task_queue[QUEUE_SIZE];
int queue_head = 0;
int queue_tail = 0;
int task_count = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

void *worker_thread(void *arg) {
    int sockfd = *((int *)arg);
    
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (task_count == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        
        Task task = task_queue[queue_head];
        queue_head = (queue_head + 1) % QUEUE_SIZE;
        task_count--;
        pthread_mutex_unlock(&queue_mutex);
        
        // Обработка запроса
        time_t raw_time;
        struct tm *time_info;
        char time_str[100];
        
        time(&raw_time);
        time_info = localtime(&raw_time);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        
        sendto(sockfd, time_str, strlen(time_str), 0,
               (struct sockaddr*)&task.client_addr, task.addr_len);
    }
    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    pthread_t threads[THREAD_POOL_SIZE];
    
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
    
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, worker_thread, &sockfd);
    }
    
    printf("Параллельный UDP сервер запущен на порту %d\n", PORT);
    while (1) {
        Task new_task;
        new_task.addr_len = sizeof(new_task.client_addr);
        
        ssize_t recv_len = recvfrom(sockfd, new_task.buffer, sizeof(new_task.buffer), 0,
                                    (struct sockaddr*)&new_task.client_addr, &new_task.addr_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        new_task.len = recv_len;
        new_task.buffer[recv_len] = '\0';
        
        pthread_mutex_lock(&queue_mutex);
        if (task_count >= QUEUE_SIZE) {
            printf("Очередь задач переполнена\n");
            pthread_mutex_unlock(&queue_mutex);
            continue;
        }
        
        task_queue[queue_tail] = new_task;
        queue_tail = (queue_tail + 1) % QUEUE_SIZE;
        task_count++;
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&queue_mutex);
    }
    
    close(sockfd);
    return 0;
}