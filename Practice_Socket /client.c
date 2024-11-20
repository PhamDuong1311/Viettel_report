#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/sha.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

void calculate_sha256(const char *filename, unsigned char *hash) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File not found");
        return;
    }

    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        SHA256_Update(&sha256_ctx, buffer, bytes_read);
    }

    SHA256_Final(hash, &sha256_ctx);
    fclose(file);
}

void receive_file(int server_socket, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error creating file");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("File received: %s\n", filename);
}

void list_files(int server_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    printf("Available files:\n");
    while ((bytes_received = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';
        if (strcmp(buffer, "END_OF_LIST\n") == 0) {
            break;
        }
        printf("%s", buffer);
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Tạo socket TCP
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

    // Kết nối đến server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        exit(1);
    }

    // Lấy danh sách file từ server
    list_files(client_socket);

    // Nhập tên file cần tải về
    char filename[BUFFER_SIZE];
    printf("Enter the file name to download: ");
    scanf("%s", filename);

    // Gửi yêu cầu tải file về server
    send(client_socket, filename, strlen(filename), 0);

    // Nhận và lưu file từ server
    receive_file(client_socket, filename);

    // Kiểm tra tính toàn vẹn của file
    unsigned char hash[SHA256_DIGEST_LENGTH];
    calculate_sha256(filename, hash);

    printf("SHA256 checksum of the received file: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    // Đóng kết nối
    close(client_socket);

    return 0;
}
