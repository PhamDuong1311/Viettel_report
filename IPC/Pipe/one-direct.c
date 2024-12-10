#include <stdio.h>
#include <unistd.h>

int main() {
    int fds[2];
    pid_t pid;
    char writemsg[2][10] = {"Viettel1", "Viettel2"};
    char readmsg[2][10];

    if (pipe(fds) == -1) {
        printf("Failed for pipe");
        return 1;
    }

    pid = fork();

    if (pid > 0) {
        read(fds[0], readmsg[0], sizeof(readmsg[0]));
        read(fds[0], readmsg[1], sizeof(readmsg[1]));
        printf("Parent process reads from pipe 2 messages: %s, %s\n", readmsg[0], readmsg[1]);
    }
    else if (pid == 0) {
        write(fds[1], writemsg[0], sizeof(writemsg[0]));
        write(fds[1], writemsg[1], sizeof(writemsg[1]));
        printf("Child process writes to pipe 2 messages: %s, %s\n", writemsg[0], writemsg[1]);
    }

    return 0;
}