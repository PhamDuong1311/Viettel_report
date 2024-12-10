#include <stdio.h>
#include <unistd.h>

int main() {
    int fds1[2], fds2[2];
    pid_t pid;
    char writemsg[2][10] = {"hehe", "haha"};
    char readmsg[2][10];

    if (pipe(fds1) == -1) {
        printf("failed for pipe 1");
        return 1;
    }

    if (pipe(fds2) == -1) {
        printf("failed for pipe 2");
        return 1;
    }

    pid = fork();

    if (pid == 0) {
        close(fds1[0]);
        close(fds2[1]);
        write(fds1[1], writemsg[0], sizeof(writemsg[0]));
        printf("Child process write to pipe 1: %s\n", writemsg[0]);
        read(fds2[0], readmsg[1], sizeof(readmsg[0]));
        printf("Child process read from pipe 2: %s\n", readmsg[1]);

    }
    else if (pid > 0) {
        close(fds1[1]);
        close(fds2[0]);
        read(fds1[0], readmsg[0], sizeof(readmsg[0]));
        printf("Parent process read from pipe 1: %s\n", readmsg[0]);
        write(fds2[1], writemsg[1], sizeof(writemsg[1]));
        printf("Parent process write to pipe 2: %s\n", writemsg[1]);

    }
    return 0;
}