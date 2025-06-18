#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }
    
    printf("PID: %d (Ожидание SIGUSR1)\n", getpid());
    fflush(stdout);
    
    while(1) {
        int sig;
        if (sigwait(&mask, &sig)) {
            perror("sigwait");
            return 1;
        }
        if (sig == SIGUSR1) {
            printf("Получен SIGUSR1\n");
            fflush(stdout);
        }
    }
    return 0;
}