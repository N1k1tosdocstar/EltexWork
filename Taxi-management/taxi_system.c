#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>

#define MAX_DRIVERS 100
#define BUFFER_SIZE 256
#define SOCKET_PATH_TEMPLATE "/tmp/taxi_driver_%d.sock"

typedef enum {
    AVAILABLE,
    BUSY
} DriverState;

typedef struct {
    pid_t pid;
    char socket_path[128];
} DriverInfo;

DriverInfo drivers[MAX_DRIVERS];
int num_drivers = 0;

void driver_main(char *socket_path) {
    int sockfd, client_fd;
    struct sockaddr_un addr;
    fd_set readfds;
    DriverState state = AVAILABLE;
    time_t end_time = 0;
    
    // Создаем сокет
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    
    unlink(socket_path);
    
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        if (state == BUSY && time(NULL) >= end_time) {
            state = AVAILABLE;
        }
        
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        struct timeval timeout = {0, 100000}; // Таймаут 100 мс
        int activity = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        
        if (activity < 0 && errno != EINTR) {
            perror("select");
        }
        
        if (activity > 0 && FD_ISSET(sockfd, &readfds)) {
            if ((client_fd = accept(sockfd, NULL, NULL)) < 0) {
                perror("accept");
                continue;
            }
            
            char buffer[BUFFER_SIZE];
            int n = read(client_fd, buffer, BUFFER_SIZE - 1);
            if (n > 0) {
                buffer[n] = '\0';
                
                if (strncmp(buffer, "send_task", 9) == 0) {
                    double task_timer;
                    if (sscanf(buffer + 10, "%lf", &task_timer) == 1) {
                        if (state == AVAILABLE) {
                            state = BUSY;
                            end_time = time(NULL) + (time_t)task_timer;
                            write(client_fd, "OK", 2);
                        } else {
                            time_t remaining = end_time - time(NULL);
                            if (remaining < 0) remaining = 0;
                            char resp[BUFFER_SIZE];
                            snprintf(resp, sizeof(resp), "Busy %ld", remaining);
                            write(client_fd, resp, strlen(resp));
                        }
                    } else {
                        write(client_fd, "ERROR: invalid command", 21);
                    }
                } 
                else if (strncmp(buffer, "get_status", 10) == 0) {
                    if (state == AVAILABLE) {
                        write(client_fd, "Available", 9);
                    } else {
                        time_t remaining = end_time - time(NULL);
                        if (remaining < 0) {
                            state = AVAILABLE;
                            write(client_fd, "Available", 9);
                        } else {
                            char resp[BUFFER_SIZE];
                            snprintf(resp, sizeof(resp), "Busy %ld", remaining);
                            write(client_fd, resp, strlen(resp));
                        }
                    }
                } 
                else if (strncmp(buffer, "exit", 4) == 0) {
                    close(client_fd);
                    close(sockfd);
                    unlink(socket_path);
                    exit(EXIT_SUCCESS);
                } 
                else {
                    write(client_fd, "ERROR: unknown command", 22);
                }
            }
            close(client_fd);
        }
    }
}

void send_command_to_driver(pid_t pid, const char* command, char* response, size_t resp_size) {
    for (int i = 0; i < num_drivers; i++) {
        if (drivers[i].pid == pid) {
            int sockfd;
            struct sockaddr_un addr;
            
            if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
                perror("socket");
                return;
            }
            
            memset(&addr, 0, sizeof(addr));
            addr.sun_family = AF_UNIX;
            strncpy(addr.sun_path, drivers[i].socket_path, sizeof(addr.sun_path) - 1);
            
            if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
                perror("connect");
                close(sockfd);
                return;
            }
            
            write(sockfd, command, strlen(command));
            
            int n = read(sockfd, response, resp_size - 1);
            if (n > 0) {
                response[n] = '\0';
            } else {
                response[0] = '\0';
            }
            
            close(sockfd);
            return;
        }
    }
    snprintf(response, resp_size, "Driver with PID %d not found", pid);
}

void create_driver() {
    if (num_drivers >= MAX_DRIVERS) {
        printf("Cannot create more drivers\n");
        return;
    }
    
    pid_t pid = fork();
    if (pid == 0) { //(водитель)
        char socket_path[128];
        snprintf(socket_path, sizeof(socket_path), SOCKET_PATH_TEMPLATE, getpid());
        driver_main(socket_path);
        exit(EXIT_SUCCESS);
    } 
    else if (pid > 0) { //(CLI)
        drivers[num_drivers].pid = pid;
        snprintf(drivers[num_drivers].socket_path, 
                sizeof(drivers[num_drivers].socket_path),
                SOCKET_PATH_TEMPLATE, pid);
        num_drivers++;
        printf("Driver created with PID: %d\n", pid);
    } 
    else {
        perror("fork");
    }
}

void send_task(pid_t pid, double task_timer) {
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "send_task %f", task_timer);
    
    char response[BUFFER_SIZE];
    send_command_to_driver(pid, command, response, sizeof(response));
    printf("%s\n", response);
}

void get_status(pid_t pid) {
    char response[BUFFER_SIZE];
    send_command_to_driver(pid, "get_status", response, sizeof(response));
    printf("%s\n", response);
}

void get_drivers() {
    if (num_drivers == 0) {
        printf("No drivers available\n");
        return;
    }
    
    for (int i = 0; i < num_drivers; i++) {
        char response[BUFFER_SIZE];
        send_command_to_driver(drivers[i].pid, "get_status", response, sizeof(response));
        printf("Driver %d: %s\n", drivers[i].pid, response);
    }
}

void cleanup() {
    for (int i = 0; i < num_drivers; i++) {
        char response[BUFFER_SIZE];
        send_command_to_driver(drivers[i].pid, "exit", response, sizeof(response));
        
        int status;
        waitpid(drivers[i].pid, &status, 0);
    }
    num_drivers = 0;
}

int main() {
    char input[BUFFER_SIZE];
    
    printf("Taxi Management System\n");
    printf("Commands: create_driver, send_task <pid> <timer>, get_status <pid>, get_drivers, exit\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) break;
        
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "create_driver") == 0) {
            create_driver();
        }
        else if (strncmp(input, "send_task", 9) == 0) {
            pid_t pid;
            double task_timer;
            if (sscanf(input + 10, "%d %lf", &pid, &task_timer) == 2) {
                send_task(pid, task_timer);
            } else {
                printf("Invalid command format. Use: send_task <pid> <timer>\n");
            }
        }
        else if (strncmp(input, "get_status", 10) == 0) {
            pid_t pid;
            if (sscanf(input + 11, "%d", &pid) == 1) {
                get_status(pid);
            } else {
                printf("Invalid command format. Use: get_status <pid>\n");
            }
        }
        else if (strcmp(input, "get_drivers") == 0) {
            get_drivers();
        }
        else if (strcmp(input, "exit") == 0) {
            cleanup();
            break;
        }
        else {
            printf("Unknown command: %s\n", input);
            printf("Available commands: create_driver, send_task <pid> <timer>, get_status <pid>, get_drivers, exit\n");
        }
    }
    
    return 0;
}