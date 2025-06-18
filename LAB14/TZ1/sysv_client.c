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
    int msgid = msgget(key, 0666);
    
    struct msgbuf msg;
    // Получение сообщения
    msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
    printf("Client received: %s\n", msg.mtext);
    
    // Отправка ответа
    msg.mtype = 2;
    strcpy(msg.mtext, "Hello!");
    msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0);
    
    return 0;
}