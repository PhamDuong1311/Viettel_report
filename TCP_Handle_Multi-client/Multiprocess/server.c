#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define FILE_DIR "./file_server"

void send_file(int sock, const char *filename) {
    char filepath[1024];
    size_t read_byte;
    char buffer[1024];

    snprintf(filepath, sizeof(filepath), "%s/%s", FILE_DIR, filename);

    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("fopen()");
        return;
    }

    while ((read_byte = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(sock, buffer, read_byte, 0) == -1) {
            perror("send()");
            fclose(file);
            return;
        }
    }
    fclose(file);
    printf("File %s has been sent.\n", filename);
}

void handle_client(int sock) {
    ssize_t byte_received;
    char buffer[1024];

    if ((byte_received = recv(sock, buffer, sizeof(buffer), 0)) <= 0) {
        perror("recv()");
        return;
    }
    buffer[byte_received] = '\0';
    printf("Client requested file: %s\n", buffer);
    send_file(sock, buffer);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    pid_t child;
    
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind()");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen()");
        exit(1);
    }

    printf("Server is listening on port: %d...\n", PORT);

    int count = 0;
    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("accept()");
            exit(1);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("Quantity of connected clients: %d\n", ++count);

        if ((child = fork()) == 0) {
            close(server_socket);
            handle_client(client_socket);
            close(client_socket);
            exit(0);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}