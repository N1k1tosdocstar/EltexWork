#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    const char *fifo = "/tmp/myfifo";
    int fd = open(fifo, O_RDONLY);
    char buffer[20];

    read(fd, buffer, sizeof(buffer));
    printf("Received: %s\n", buffer);
    close(fd);
    unlink(fifo);
    return 0;
}
//gcc client.c -o client && ./client