#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <openssl/sha.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_DIR "./binary_file/"

void send_file(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File not found");
        return;
    }

    // Đọc file và gửi từng phần
    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, bytes, 0) == -1) {
            perror("Send file failed");
            break;
        }
    }
    fclose(file);
}

void list_files(int client_socket) {
    DIR *dir = opendir(FILE_DIR);
    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }

    struct dirent *entry;
    char buffer[BUFFER_SIZE];
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            snprintf(buffer, sizeof(buffer), "%s\n", entry->d_name);
            send(client_socket, buffer, strlen(buffer), 0);
        }
    }

    // Gửi thông báo kết thúc danh sách file
    snprintf(buffer, sizeof(buffer), "END_OF_LIST\n");
    send(client_socket, buffer, strlen(buffer), 0);
    closedir(dir);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Gửi danh sách các file
    list_files(client_socket);

    // Nhận tên file từ client và gửi file
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        return;
    }
    buffer[bytes_received] = '\0';
    printf("Requested file: %s\n", buffer);

    char filepath[BUFFER_SIZE];
    snprintf(filepath, sizeof(filepath), "%s%s", FILE_DIR, buffer);
    send_file(client_socket, filepath);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Tạo socket TCP
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Liên kết socket với địa chỉ và cổng
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    // Lắng nghe kết nối từ client
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Chấp nhận kết nối từ client
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
        perror("Accept failed");
        exit(1);
    }

    // Xử lý yêu cầu từ client
    handle_client(client_socket);

    // Đóng kết nối
    close(client_socket);
    close(server_socket);

    return 0;
}
