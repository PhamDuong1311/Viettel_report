#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

void receive_file(int server_sock, const char *filename) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    FILE *file;

    file = fopen(filename, "wb");
    if (file == NULL) {
        perror("fopen failed");
        return;
    }

    while ((bytes_received = recv(server_sock, buffer, sizeof(buffer), 0)) > 0) {
        if (strncmp(buffer, "END", 3) == 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
    }

    printf("Tải file %s thành công\n", filename);
    fclose(file);
}

void request_file_list(int server_sock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while (1) {
        bytes_received = recv(server_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            printf("Không nhận được dữ liệu\n");
            return;
        }

        buffer[bytes_received] = '\0';
        if (strncmp(buffer, "END", 3) == 0) {
            break;
        }

        printf("File có sẵn: %s\n", buffer);
    }
}


int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char filename[BUFFER_SIZE];

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        exit(1);
    }

    printf("Kết nối thành công tới server\n");

    request_file_list(client_socket);

    printf("Nhập tên file bạn muốn tải về: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';  

    send(client_socket, filename, strlen(filename), 0);

    receive_file(client_socket, filename);

    close(client_socket);

    return 0;
}
