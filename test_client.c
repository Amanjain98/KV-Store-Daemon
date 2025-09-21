#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/kv_socket"

void send_command(const char *cmd) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    int ret = write(sock, cmd, strlen(cmd));
    if (ret <= 0) {
        perror("write failed");
        exit(1);
    }
    char buffer[512] = {0};
    int bytes = read(sock, buffer, sizeof(buffer));
    buffer[bytes] = '\0';
    printf("Response: %s\n", buffer);
    close(sock);
}

int main() {
    send_command("PUT name Aarav");
    send_command("GET name");
    send_command("DELETE name");
    send_command("GET name");
    return 0;
}