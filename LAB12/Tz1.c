#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int fd[2];
    pid_t pid;
    char buffer[20];
    if (pipe(fd)== -1){
        perror ("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1){
        perror ("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0){
        close(fd[0]);
        write(fd[1], "Hi!",4);
        close(fd[1]);
        wait(NULL);
    }else{
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));
        printf("Received: %s\n", buffer);
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}
//gcc Tz1.c -o TZ && ./TZ