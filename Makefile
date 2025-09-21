CC = gcc
CFLAGS = -Wall -pthread -O2
TARGET = kv_daemon

SRCS = main.c ipc_server.c thread_pool.c kv_store.c block_store.c
HDRS = ipc_server.h thread_pool.h kv_store.h block_store.h

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

test_client:
	$(CC) $(CFLAGS) -o test_client test_client.c

clean:
	rm -f $(TARGET)