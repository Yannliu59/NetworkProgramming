//
// Created by chengzi on 2022/7/20.
//
// p28 共享内存介绍（一）

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int fd;
    // O_CREAT │若文件存在，此标志无用；若不存在，建新文件
    // O_RDWR  │即读也写
    // O_TRUNC │若文件存在，则长度被截为0，属性不变
    fd = open(argv[1], O_RDWR, 0666);
    if (fd == -1)
    {
        ERR_EXIT("open");
    }

    STU *p;
    p = (STU*)mmap(NULL, sizeof(STU) * 10, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == NULL)
        ERR_EXIT("mmap");

    int i;
    for (int i = 0; i < 10; i++)
    {
        printf("name = %s age = %d\n", (p + i) -> name, (p + i)  -> age);
    }
    munmap(p, sizeof(STU) * 10);
    printf("exit......\n");
    return 0;
}