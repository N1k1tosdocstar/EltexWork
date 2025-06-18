#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct msgbuf {
    long mtype;
    char mtext[256];
};

int main() {
    key_t key = ftok("server", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    
    struct msgbuf msg = {.mtype = 1};
    strcpy(msg.mtext, "Hi!");
    
    // Отправка сообщения
    msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0);
    
    // Получение ответа
    msgrcv(msgid, &msg, sizeof(msg.mtext), 2, 0);
    printf("Server received: %s\n", msg.mtext);
    
    // Удаление очереди
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}