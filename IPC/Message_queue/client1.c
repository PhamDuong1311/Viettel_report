#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include <fcntl.h>
#include <mqueue.h>

int main() {
    mqd_t qd;
    char in_buffer1[256];
    if ((qd = mq_open("/my_mq", O_RDONLY)) == -1) {
        perror("Client: mq_open");
        exit(1);
    }

    if (mq_receive(qd, in_buffer1, sizeof(in_buffer1), NULL) == -1) {
        perror("CLient: mq_receive");
        exit(1);
    }

    printf("Client received string: %s\n", in_buffer1);


    return 0;
}