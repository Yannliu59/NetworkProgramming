//
// Created by chengzi on 2022/7/21.
//
// 读共享内存程序

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
    shmid = shmget(1234, 0, 0);
    if (shmid == -1)
        ERR_EXIT("shmget");

    // 第一次创建完共享内存时，它还不能被任何进程访问，shmat()函数的作用就是用来启动对该共享内存的访问，并把共享内存连接到当前进程的地址空间
    // 将共享内存链接到当前进程的地址空间
    STU *shm;
    shm = (struct stu*)shmat(shmid, 0, 0);
    if (shm == (void *)-1)
    {
        ERR_EXIT("shmat");
    }

    //设置共享内存
    stu *p = (struct stu*)shm;
    printf("student name : %s, age: %d\n", p -> name, p -> age);

    memcpy(p, "quit", 4);

    if (shmdt(shm) == -1)
    {
        ERR_EXIT("shmdt");
    }
    return 0;
}