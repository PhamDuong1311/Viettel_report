#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9092
#define BUFFER_SIZE 1024
#define FILE_DIR "./binary_file_server"

void send_file(int server_socket, struct sockaddr_in server_addr, const char *filename) {
    char filepath[BUFFER_SIZE];
    FILE *file;
    size_t bytes_read;
    char buffer[BUFFER_SIZE];
    socklen_t server_len = sizeof(server_addr);

    snprintf(filepath, sizeof(filepath), "%s/%s", FILE_DIR, filename);

    file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("fopen failed");
        sendto(server_socket, "ERROR\n", 6, 0, (struct sockaddr*)&server_addr, server_len);
        return;
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (sendto(server_socket, buffer, bytes_read, 0, (struct sockaddr*)&server_addr, server_len) == -1) {
            perror("sendto failed");
            fclose(file);
            return;
        }
    }

    sendto(server_socket, "END", 3, 0, (struct sockaddr*)&server_addr, server_len);
    fclose(file);
    printf("File %s has been sent.\n", filename);
}

void handle_client(int server_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);

    bytes_received = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&server_addr, &server_len);
    if (bytes_received <= 0) {
        perror("recvfrom failed");
        return;
    }
    buffer[bytes_received] = '\0';  

    printf("Client requested file: %s\n", buffer);

    send_file(server_socket, server_addr, buffer);
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *)&server_addr, addrlen) == -1) {
        perror("bind failed");
        exit(1);
    }

    printf("Server's listening on port %d...\n", PORT);

    handle_client(server_socket);

    close(server_socket);

    return 0;
}
