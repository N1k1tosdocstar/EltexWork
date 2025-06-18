#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>  // Добавим задержку

#define QUEUE_NAME "/posix_queue"

int main() {
    mqd_t mq;
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 256,
        .mq_curmsgs = 0
    };

    // Удаляем старую очередь если существует
    mq_unlink(QUEUE_NAME);
    
    // Создаем новую очередь
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open failed");
        exit(1);
    }
    printf("Server: Queue created successfully\n");

    // Отправляем сообщение
    char *msg = "Hi!";
    if (mq_send(mq, msg, strlen(msg) + 1, 0) == -1) {
        perror("mq_send failed");
    } else {
        printf("Server: Sent message: %s\n", msg);
    }

    // Даем время клиенту запуститься
    printf("Server: Waiting for client...\n");
    sleep(5);

    // Получаем ответ
    char buffer[256];
    ssize_t bytes = mq_receive(mq, buffer, sizeof(buffer), NULL);
    if (bytes == -1) {
        perror("mq_receive failed");
    } else {
        printf("Server received: %s\n", buffer);
    }

    // Закрываем и удаляем очередь
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    printf("Server: Queue closed\n");
    return 0;
}