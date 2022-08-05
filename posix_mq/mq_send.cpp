//
// Created by chengzi on 2022/7/26.
//
#include <cstring>
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

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr,"Usage: %s <prio>", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd_t mqid;
    mqid = mq_open("/abc", O_WRONLY);
    if (mqid == (mqd_t)-1)
    {
        ERR_EXIT("mq_open");
    }
    printf("mq_open success\n");

    STU stu;
    strcpy(stu.name, "zhangsan");
    stu.age = 20;
    unsigned prio = atoi(argv[1]);
    mq_send(mqid, (const char*)&stu, sizeof(STU),prio);
    mq_close(mqid);
    return 0;
}