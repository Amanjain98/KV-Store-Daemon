#ifndef THREAD_POOL_H
#define THREAD_POOL_H

void init_thread_pool();
void destroy_thread_pool();
void submit_to_thread_pool(int client_fd, int block_fd);

#endif