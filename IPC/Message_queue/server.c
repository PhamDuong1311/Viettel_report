#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <mqueue.h>

int main() {
    mqd_t qd;
    char out_buffer[256] = "message of server";

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    if ((qd = mq_open("/my_mq", O_WRONLY | O_CREAT, 0660, &attr)) == -1) {
        perror("Server: mq_open");
        exit(1);
    }
    int count = 2;
    while (count > 0) {
        if (mq_send(qd, out_buffer, sizeof(out_buffer), 0)) {
            perror("Server: mq_send");
            exit(1);
        }

        printf("Server: Message sent string %s.\n", out_buffer);
        count--;
    }
    return 0;
}