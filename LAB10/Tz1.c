#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Ошибка при создании процесса
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        exit(EXIT_SUCCESS);
    } else {
        // Родительский процесс
        printf("Родительский процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        int status;
        wait(&status);  // Ожидание завершения дочернего процесса
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился со статусом: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
