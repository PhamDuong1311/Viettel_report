#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9092
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define FILE_DIR_CLIENT "./binary_file_client"

void receive_file(int client_sock, struct sockaddr_in *server_addr, socklen_t addr_len, const char *filename) {
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

    while (1) {
        bytes_received = recvfrom(client_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)server_addr, &addr_len);
        if (bytes_received <= 0) {
            perror("recvfrom failed");
            break;
        }

        if (strncmp(buffer, "END", 3) == 0) {
            break;
        }

        fwrite(buffer, 1, bytes_received, file);
    }

    printf("File %s downloaded successfully\n", filename);
    fclose(file);
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char filename[BUFFER_SIZE];

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    printf("Enter the filename you want to download: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';  

    if (sendto(client_socket, filename, strlen(filename), 0, (struct sockaddr *)&server_addr, addr_len) == -1) {
        perror("sendto failed");
        close(client_socket);
        exit(1);
    }

    receive_file(client_socket, &server_addr, addr_len, filename);

    close(client_socket);

    return 0;
}
