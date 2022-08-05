//
// Created by chengzi on 2022/7/26.
//
#include <csignal>
#include <sys/mman.h>
#include <cstring>
#include "unistd.h"
#include "sys/types.h"
#include "fcntl.h"
#include "sys/stat.h"


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

int main()
{
    int shmid;
    shmid = shm_open("/xyz", O_RDWR, 0);
    if (shmid == -1)
        ERR_EXIT("shm_open");
    printf("share memory %d create success\n", shmid);

    struct stat buf;
    if (fstat(shmid, &buf) == -1)
        ERR_EXIT("fstat");

    STU *p;
    p = static_cast<STU *>(mmap(NULL, buf.st_size, PROT_WRITE, MAP_SHARED, shmid, 0));
    if (p == MAP_FAILED)
        ERR_EXIT("mmap");

    strcpy(p->name, "zhangsan");
    p->age = 20;

    close( shmid);
    return 0;
}