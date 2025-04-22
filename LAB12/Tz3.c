#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 10
#define MAX_CMDS 5

void parser_com(char *cmd, char **arrgs){
    int i =0;
    arrgs [i] = strtok(cmd, " ");
    while(arrgs[i] != NULL && i < MAX_ARGS-1) { 
        arrgs[++i] = strtok(NULL, " ");
    }
    arrgs[i]=NULL;
}
int main() {
    char input[100];
    char *cmds[MAX_CMDS];
    int num = 0;
    int i;
    int prev_pipe = -1;
    int fd[2];

    printf("> ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    cmds[num] = strtok(input, "|");
    while (cmds[num] != NULL && num < MAX_CMDS-1) {
        cmds[++num] = strtok(NULL, "|");
    }

    for (i = 0; i < num; i++) {
        if (i < num - 1) {
            pipe(fd);
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe);
            }

            if (i < num - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
            }

            char *args[MAX_ARGS];
            parser_com(cmds[i], args);
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            if (i > 0) {
                close(prev_pipe);
            }
            if (i < num - 1) {
                prev_pipe = fd[0];
                close(fd[1]);
            }
        } else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    while (wait(NULL) > 0);

    return 0;
}
//gcc Tz3.c -o myshell && ./myshell