#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    const char *fifo = "/tmp/myfifo";
    mkfifo(fifo, 0666);

    int fd = open(fifo, O_WRONLY);
    write(fd, "Hi!",4);
    close(fd);
    return 0;
}
//gcc server.c -o server && ./server