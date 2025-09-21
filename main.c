#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include "ipc_server.h"
#include "block_store.h"

volatile sig_atomic_t running = 1;

void handle_sigint(int sig) {
    running = 0;
}

int main() {
    signal(SIGINT, handle_sigint);

    int fd = open("/tmp/kv_data.bin", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    init_block_store(fd);
    start_ipc_server(fd);

    close(fd);
    return 0;
}