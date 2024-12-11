#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    int shm_fd;
    void *ptr;
    char message_1[256] = "VHT ";
    char message_2[256] = "viettel";

    if ((shm_fd = shm_open("my_shm", O_CREAT | O_RDWR, 0666)) == -1) {
        perror("Server: shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, 4096) == -1) {
        perror("Server: ftruncate");
        exit(1);
    }

    ptr = mmap(0, 4096, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    void *start = ptr;
    sprintf(ptr, "%s", message_1);
    ptr += strlen(message_1);
    sprintf(ptr, "%s", message_2);
    ptr += strlen(message_2);
    printf("Server write to shared memory: %s\n", (char *)start);

    printf("Server read from shared memory: %s\n", (char *)start);
    return 0;
}