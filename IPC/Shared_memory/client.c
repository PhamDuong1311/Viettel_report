#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    int shm_fd;
    void *ptr;

    if ((shm_fd = shm_open("/my_shm", O_RDONLY, 0666)) == -1) {
        perror("Client: shm_open");
        exit(1);
    }
    ptr = mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0);
    printf("Client read from shared memory: %s\n", (char *)ptr);

    shm_unlink("my_shm");
    return 0;
}