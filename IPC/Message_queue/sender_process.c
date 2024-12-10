#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char sendmsg[1024] = "RapViet ss4";
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;
    attr.mq_curmsgs = 0;

    mq = mq_open("/my_queue", O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    if (mq_send(mq, sendmsg, sizeof(sendmsg), 0) == -1) {
        perror("mq_send");
        exit(1);
    }

    return 0;
}