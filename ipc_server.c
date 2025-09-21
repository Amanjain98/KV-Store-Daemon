#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>
#include "kv_store.h"

#define SOCKET_PATH "/tmp/kv_socket"

extern volatile sig_atomic_t running;

typedef struct {
    int client_fd;
    int block_fd;
} client_args;

void* client_thread(void* arg) {
    client_args* args = (client_args*)arg;
    handle_client(args->client_fd, args->block_fd);
    close(args->client_fd);
    free(args);
    return NULL;
}

void start_ipc_server(int block_fd) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 ||
        listen(server_fd, 16) < 0) {
        perror("bind/listen");
        exit(EXIT_FAILURE);
    }

    while (running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        client_args* args = malloc(sizeof(client_args));
        args->client_fd = client_fd;
        args->block_fd = block_fd;

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, args);
        pthread_detach(tid);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
}