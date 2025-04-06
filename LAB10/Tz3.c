#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 100

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_INPUT_SIZE / 2 + 1];
    int status;

    while (1) {
        printf("Введите команду (или 'exit' для выхода): ");
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break;  // В случае ошибки ввода или EOF
        }

        // Удаление символа новой строки
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Разбиение строки на аргументы
        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " ");
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Дочерний процесс
            execvp(args[0], args);
            perror("execvp");  // Если execvp вернул ошибку
            exit(EXIT_FAILURE);
        } else {
            // Родительский процесс
            wait(&status);  // Ожидание завершения дочернего процесса
            if (WIFEXITED(status)) {
                printf("Дочерний процесс завершился со статусом: %d\n", WEXITSTATUS(status));
            }
        }
    }

    return 0;
}