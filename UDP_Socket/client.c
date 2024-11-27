#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 8080
#define CLIENT_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define FILE_DIR_CLIENT "./binary_file_client"

void receive_file(int client_sock, const char *filename) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    FILE *file;
    char filepath[BUFFER_SIZE];

    snprintf(filepath, sizeof(filepath), "%s/%s", FILE_DIR_CLIENT, filename);

    file = fopen(filepath, "wb");
    if (file == NULL) {
        perror("fopen failed");
        return;
    }

    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
        if (strncmp(buffer, "END", 3) == 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
    }

    printf("Tải file %s thành công\n", filename);
    fclose(file);
}

int handle_server(int client_socket, const char *filename) {
    printf("Nhập tên file bạn muốn tải về: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';  

    send(client_socket, filename, strlen(filename), 0);

    receive_file(client_socket, filename);
}

int main() {
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    client_addr = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_addr == -1) {
        perror("socket failed");
        exit(1);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, CLIENT_IP, &client_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(1);
    }

    handle_server(client_socket, &filename);

    close(client_sock);

    return 0;
}