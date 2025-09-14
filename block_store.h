#ifndef BLOCK_STORE_H
#define BLOCK_STORE_H

#define BLOCK_SIZE 256
#define TOTAL_BLOCKS (64 * 1024 * 1024 / BLOCK_SIZE)
#define BITMAP_SIZE (TOTAL_BLOCKS / 8)
#define INDEX_SIZE 1024

typedef struct {
    char key[128];
    int block_num;
} index_entry;

void init_block_store(int fd);
int find_free_block();
void free_block(int block);
void update_index(const char* key, int block);
int get_index_block(const char* key);
void remove_index(const char* key);
void write_kv_to_block(int block, const char* key, const char* value);
char* read_value(int block);

#endif