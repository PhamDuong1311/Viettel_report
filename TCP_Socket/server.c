#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_DIR "./binary_file_server"


void send_file(int client_sock, const char *filename) {
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
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int opt = 1;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(1);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        exit(1);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server đang lắng nghe ở cổng %d...\n", PORT);

    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
        perror("Accept failed");
        exit(1);
    }

    printf("Chấp nhận kết nối và tạo 1 socket mới cho client\n");
 
    handle_client(client_socket);

    close(client_socket);
    close(server_socket);

    return 0;
}
