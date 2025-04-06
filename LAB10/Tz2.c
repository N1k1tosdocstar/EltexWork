#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void create_process(int process_number) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Процесс %d: PID = %d, PPID = %d\n", process_number, getpid(), getppid());
        exit(EXIT_SUCCESS);
    } else {
        // Родительский процесс
        wait(NULL);  // Ожидание завершения дочернего процесса
    }
}

int main() {
    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Процесс1
        printf("Процесс1: PID = %d, PPID = %d\n", getpid(), getppid());
        create_process(3);
        create_process(4);
        exit(EXIT_SUCCESS);
    } else {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid2 == 0) {
            // Процесс2
            printf("Процесс2: PID = %d, PPID = %d\n", getpid(), getppid());
            create_process(5);
            exit(EXIT_SUCCESS);
        } else {
            // Родительский процесс
            wait(NULL);  // Ожидание завершения процесс1
            wait(NULL);  // Ожидание завершения процесс2
        }
    }

    return 0;
}
