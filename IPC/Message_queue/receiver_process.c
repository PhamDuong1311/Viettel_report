#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    mqd_t mq;
    char receivemsg[1024];
    
    mq = mq_open("/my_queue", O_RDONLY);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    if (mq_receive(mq, receivemsg, sizeof(receivemsg), NULL) == -1) {
        perror("mq_receive");
        exit(1);
    }

    printf("Received: %s\n", receivemsg);

    if (mq_close(mq) == -1) {
        perror("mq_close");
        exit(1);
    }

    if (mq_unlink("/my_queue") == -1) {
        perror("mq_unlink");
        exit(1);
    }
    return 0;
}