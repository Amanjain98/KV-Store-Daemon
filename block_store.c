#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "block_store.h"

static void* mapped_file = NULL;
static pthread_mutex_t store_mutex = PTHREAD_MUTEX_INITIALIZER;

static unsigned char* bitmap;
static index_entry* index_table;
static char* data_blocks;

void init_block_store(int fd) {
    ftruncate(fd, 64 * 1024 * 1024);
    mapped_file = mmap(NULL, 64 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    bitmap = (unsigned char*)mapped_file;
    index_table = (index_entry*)(bitmap + BITMAP_SIZE);
    data_blocks = (char*)(bitmap + BITMAP_SIZE + sizeof(index_entry) * INDEX_SIZE);
}

int find_free_block() {
    pthread_mutex_lock(&store_mutex);
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        int byte = i / 8, bit = i % 8;
        if (!(bitmap[byte] & (1 << bit))) {
            bitmap[byte] |= (1 << bit);
            pthread_mutex_unlock(&store_mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&store_mutex);
    return -1;
}

void free_block(int block) {
    pthread_mutex_lock(&store_mutex);
    int byte = block / 8, bit = block % 8;
    bitmap[byte] &= ~(1 << bit);
    pthread_mutex_unlock(&store_mutex);
}

int hash_key(const char* key) {
    int hash = 0;
    for (int i = 0; key[i]; ++i)
        hash = (hash * 31 + key[i]) % INDEX_SIZE;
    return hash;
}

void update_index(const char* key, int block) {
    int idx = hash_key(key);
    pthread_mutex_lock(&store_mutex);
    strncpy(index_table[idx].key, key, sizeof(index_table[idx].key) - 1);
    index_table[idx].block_num = block;
    pthread_mutex_unlock(&store_mutex);
}

int get_index_block(const char* key) {
    int idx = hash_key(key);
    return (strcmp(index_table[idx].key, key) == 0) ? index_table[idx].block_num : -1;
}

void remove_index(const char* key) {
    int idx = hash_key(key);
    pthread_mutex_lock(&store_mutex);
    if (strcmp(index_table[idx].key, key) == 0) {
        index_table[idx].key[0] = '\0';
        index_table[idx].block_num = -1;
    }
    pthread_mutex_unlock(&store_mutex);
}

void write_kv_to_block(int block, const char* key, const char* value) {
    pthread_mutex_lock(&store_mutex);
    char* ptr = data_blocks + block * BLOCK_SIZE;
    int key_len = strlen(key), val_len = strlen(value);
    memcpy(ptr, &key_len, sizeof(int));
    memcpy(ptr + sizeof(int), &val_len, sizeof(int));
    memcpy(ptr + 2 * sizeof(int), key, key_len);
    memcpy(ptr + 2 * sizeof(int) + key_len, value, val_len);
    pthread_mutex_unlock(&store_mutex);
}

char* read_value(int block) {
    pthread_mutex_lock(&store_mutex);   
    char* ptr = data_blocks + block * BLOCK_SIZE;
    int key_len, val_len;
    memcpy(&key_len, ptr, sizeof(int));
    memcpy(&val_len, ptr + sizeof(int), sizeof(int));
    char* val = malloc(val_len + 1);
    memcpy(val, ptr + 2 * sizeof(int) + key_len, val_len);
    val[val_len] = '\0';
    pthread_mutex_unlock(&store_mutex);
    return val;
}