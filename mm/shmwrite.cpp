//
// Created by chengzi on 2022/7/20.
//
//  p29 共享内存介绍（二）
//  写共享内存程序

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>

#define ERR_EXIT(m) \
        do \
        { \
             perror(m); \
             exit(EXIT_FAILURE);    \
        } while (0);

typedef struct stu
{
    char name[4];
    int age;
} STU;

int main(int argc, char** argv)
{
    int shmid;
    //创建一段共享内存
    shmid = shmget(1234,sizeof(STU), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        ERR_EXIT("shmget");
    }

    STU *p;
    p = (struct stu*)shmat(shmid, NULL, 0);
    if (p == (void *)-1)
        ERR_EXIT("shmat");
    strcpy(p->name, "list");
    p -> age = 20;
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}