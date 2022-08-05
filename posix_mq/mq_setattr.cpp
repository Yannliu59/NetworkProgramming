//
// Created by chengzi on 2022/7/26.
//
#include "unistd.h"
#include "sys/types.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "mqueue.h"

#include "stdlib.h"
#include "stdio.h"
#include "errno.h"

#define ERR_EXIT(m) \
            do \
            {       \
                perror(m);        \
                exit(EXIT_FAILURE);    \
            } while(0);

int main(void)
{
    mqd_t mqid;
    mqid = mq_open("/abc", O_RDONLY);
    if (mqid == (mqd_t)-1)
    {
        ERR_EXIT("mq_open");
    }
    printf("mq_open success\n");
    struct mq_attr newattr,oldattr;
    newattr.mq_flags = O_NONBLOCK;
    mq_setattr(mqid, &newattr, &oldattr);
    printf("max #msg = %ld\n max #bytes = %ld\n #currently on queue = %ld\n", newattr.mq_maxmsg, newattr.mq_msgsize, newattr.mq_curmsgs);
    mq_close(mqid);
    return 0;
}