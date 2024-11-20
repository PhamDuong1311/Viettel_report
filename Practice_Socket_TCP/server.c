#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_DIR "./binary_file/"

// Hàm gửi danh sách các file trong thư mục cho client
void send_file_list(int client_sock) {
    DIR *d;
    struct dirent *dir;
    char buffer[BUFFER_SIZE];

    // Mở thư mục chứa các file binary
    d = opendir(FILE_DIR);
    if (d == NULL) {
        perror("opendir failed");
        return;
    }

    // Gửi danh sách file cho client
    while ((dir = readdir(d)) != NULL) {
        // Kiểm tra nếu đó là file
        if (dir->d_type == DT_REG) {
            snprintf(buffer, sizeof(buffer), "%s\n", dir->d_name);
            send(client_sock, buffer, strlen(buffer), 0);
        }
    }
    // Đánh dấu kết thúc danh sách
    send(client_sock, "END", 3, 0);
    closedir(d);
}

// Hàm gửi file cho client
void send_file(int client_sock, const char *filename) {
    char filepath[BUFFER_SIZE];
    FILE *file;
    size_t bytes_read;
    char buffer[BUFFER_SIZE];

    snprintf(filepath, sizeof(filepath), "%s/%s", FILE_DIR, filename);

    // Mở file để đọc
    file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("fopen failed");
        return;
    }

    // Đọc và gửi file cho client
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_sock, buffer, bytes_read, 0);
    }
    fclose(file);
    printf("File %s đã được gửi\n", filename);
}

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Gửi danh sách file cho client
    send_file_list(client_sock);

    // Nhận yêu cầu từ client (tên file cần tải)
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        printf("Nhận yêu cầu tải file từ client thất bại\n");
        return;
    }
    buffer[bytes_received] = '\0';  // Kết thúc chuỗi

    printf("Client yêu cầu tải file: %s\n", buffer);

    // Gửi file cho client
    send_file(client_sock, buffer);
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

    printf("Server đang lắng nghe ở cổng %d...\n", PORT);

    // Chấp nhận kết nối từ client
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
        perror("Accept failed");
        exit(1);
    }

    printf("Chấp nhận kết nối và tạo 1 socket mới cho client");
 
    // Xử lý yêu cầu từ client
    handle_client(client_socket);

    // Đóng kết nối
    close(client_socket);
    close(server_socket);

    return 0;
}

/**/