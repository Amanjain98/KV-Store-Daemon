#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "kv_store.h"

#define THREAD_COUNT 4
#define QUEUE_SIZE 64

typedef struct {
    int client_fd;
    int block_fd;
} task_t;

static task_t task_queue[QUEUE_SIZE];
static int head = 0, tail = 0;

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
static pthread_t threads[THREAD_COUNT];

extern volatile sig_atomic_t running;

void* worker_thread(void* arg) {
    while (running) {
        pthread_mutex_lock(&queue_mutex);

        while (head == tail && running)
            pthread_cond_wait(&queue_cond, &queue_mutex);

        if (!running) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        task_t task = task_queue[head];
        head = (head + 1) % QUEUE_SIZE;

        pthread_mutex_unlock(&queue_mutex);

        handle_client(task.client_fd, task.block_fd);
        close(task.client_fd);
    }
    return NULL;
}

void init_thread_pool() {
    for (int i = 0; i < THREAD_COUNT; ++i)
        pthread_create(&threads[i], NULL, worker_thread, NULL);
}

void destroy_thread_pool() {
    pthread_mutex_lock(&queue_mutex);
    running = 0;
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);

    for (int i = 0; i < THREAD_COUNT; ++i)
        pthread_join(threads[i], NULL);
}

void submit_to_thread_pool(int client_fd, int block_fd) {
    pthread_mutex_lock(&queue_mutex);

    task_queue[tail].client_fd = client_fd;
    task_queue[tail].block_fd = block_fd;
    tail = (tail + 1) % QUEUE_SIZE;

    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
}