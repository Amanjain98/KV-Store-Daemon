#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "kv_store.h"
#include "block_store.h"

void handle_client(int client_fd, int block_fd) {
    char buffer[512] = {0};
    int ret = read(client_fd, buffer, sizeof(buffer));
    if (ret <= 0) {
        perror("read failed");
        exit(1);
    }

    char cmd[10], key[128], value[256];
    sscanf(buffer, "%s %s %s", cmd, key, value);

    if (strcmp(cmd, "PUT") == 0) {
        int block = find_free_block();
        if (block != -1) {
            write_kv_to_block(block, key, value);
            update_index(key, block);
        }
    } else if (strcmp(cmd, "GET") == 0) {
        int block = get_index_block(key);
        char* val = (block != -1) ? read_value(block) : NULL;
        int ret = write(client_fd, val ? val : "NOT FOUND", strlen(val ? val : "NOT FOUND"));
        if (ret <= 0) {
            perror("write failed");
            exit(1);
        }
        free(val);
    } else if (strcmp(cmd, "DELETE") == 0) {
        int block = get_index_block(key);
        if (block != -1) {
            free_block(block);
            remove_index(key);
        }
    }
}