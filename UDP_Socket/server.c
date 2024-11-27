#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_DIR "./binary_file_server"

void send_file(int server_socket, const char *filename) {
    char filepath[BUFFER_SIZE];
    FILE *file;
    size_t bytes_read;
    char buffer[BUFFER_SIZE];

    snprintf(filepath, sizeof(filepath), "%s/%s", FILE_DIR, filename);

    file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("fopen failed");
        send(client_sock, "ERROR\n", 6, 0); 
        return;
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_sock, buffer, bytes_read, 0) == -1) {
            perror("send failed");
            fclose(file);
            return;
        }
    }
    fclose(file);
    printf("File %s đã được gửi\n", filename);
}

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;


    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        perror("recv failed");
        return;
    }
    buffer[bytes_received] = '\0'; 

    printf("Client yêu cầu tải file: %s\n", buffer);

    send_file(client_sock, buffer);
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

    printf("Server đang lắng nghe ở cổng %d...\n", PORT);


    handle_client(server_socket);

    close(server_socket);

    return 0;
}