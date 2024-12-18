#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define FILE_DIR_CLIENT "./file_client_2"

void receive_file(int sock, const char *filename) {
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

    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        if (strncmp(buffer, "END", 3) == 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
    }

    printf("Download file %s success\n", filename);
    fclose(file);
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

    printf("Connect to server successfully\n");

    printf("Enter the filename you want to download: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';  

    send(client_socket, filename, strlen(filename), 0);

    receive_file(client_socket, filename);
    close(client_socket);

    return 0;
}
