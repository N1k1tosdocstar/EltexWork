#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    char msg[] = "Получен SIGUSR1\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    (void)sig;
}

int main() {
    struct sigaction sa = {
        .sa_handler = handler,
        .sa_flags = 0
    };
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }
    
    printf("PID: %d\n", getpid());
    fflush(stdout); // Сброс буфера вывода
    
    while(1) pause();
    return 0;
}