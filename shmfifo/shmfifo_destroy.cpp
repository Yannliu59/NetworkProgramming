//
// Created by chengzi on 2022/7/25.
//
// p33 system V共享内存与信号量综合
// 共享环形缓冲消息队列
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "sys/sem.h"
#include "sys/shm.h"

#include "iostream"
#include "string"
#include "cstring"
#include "stdlib.h"
#include "stdio.h"
#include "errno.h"
#include "unistd.h"

#include "shmfifo.h"

#define ERR_EXIT(m) \
        do \
        {           \
            perror(m);            \
            exit(EXIT_FAILURE);        \
        }while(0);

struct student
{
    char name[32];
    int age;
};

int main(void)
{
    shmfifo_t* shmfifol = shmfifo_init(1234, sizeof(student), 3);
    shmfifo_destroy(shmfifol);
    return 0;
}