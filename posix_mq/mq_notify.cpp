//
// Created by chengzi on 2022/7/26.
//
#include <csignal>
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
typedef struct stu
{
    char name[32];
    int age;
} STU;

struct sigevent sigev;
mqd_t mqid;

void handle_sigusr1(int sig)
{
    mq_notify(mqid, &sigev);

    struct mq_attr attr;
    mq_getattr(mqid, &attr);
    size_t size = attr.mq_msgsize;

    STU stu;
    unsigned prio;

    if (mq_receive(mqid, (char*)&stu, size, &prio) == (mqd_t)-1)
    {
        ERR_EXIT("mq_receive");
    }
    printf("name = %s age = %d prio = %u\n", stu.name, stu.age, prio);
}

int main(void)
{
    mqid = mq_open("/abc", O_RDONLY);
    if (mqid == (mqd_t)-1)
    {
        ERR_EXIT("mq_open");
    }
    printf("mq_open success\n");

    signal(SIGUSR1, handle_sigusr1);

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    mq_notify(mqid, &sigev);

    for (;;)
        pause();

    mq_close(mqid);
    return 0;
}