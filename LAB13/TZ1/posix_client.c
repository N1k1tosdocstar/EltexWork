#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_NAME "/posix_queue"

int main() {
    // Даем время серверу создать очередь
    sleep(1);
    
    mqd_t mq = mq_open(QUEUE_NAME, O_RDWR);
    if (mq == (mqd_t)-1) {
        perror("mq_open failed");
        exit(1);
    }
    printf("Client: Connected to queue\n");

    // Получаем сообщение
    char buffer[256];
    ssize_t bytes = mq_receive(mq, buffer, sizeof(buffer), NULL);
    if (bytes == -1) {
        perror("mq_receive failed");
    } else {
        printf("Client received: %s\n", buffer);
    }

    // Отправляем ответ
    char *reply = "Hello!";
    if (mq_send(mq, reply, strlen(reply) + 1, 0) == -1) {
        perror("mq_send failed");
    } else {
        printf("Client: Sent reply: %s\n", reply);
    }

    mq_close(mq);
    return 0;
}