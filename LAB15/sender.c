#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <PID>\n", argv[0]);
        return 1;
    }
    
    pid_t pid = atoi(argv[1]);
    if (kill(pid, SIGUSR1) == -1) {
        perror("kill");
        return 1;
    }
    
    printf("Сигнал SIGUSR1 отправлен процессу %d\n", pid);
    return 0;
}