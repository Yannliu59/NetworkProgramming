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
    shmid = shm_open("/xyz", O_RDONLY, 0);
    if (shmid == -1)
        ERR_EXIT("shm_open");
    printf("share memory %d create success\n", shmid);

    struct stat buf;
    if (fstat(shmid, &buf) == -1)
        ERR_EXIT("fstat");

    STU *p;
    p = static_cast<STU *>(mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, shmid, 0));
    if (p == MAP_FAILED)
        ERR_EXIT("mmap");

    printf("name = %s age = %d\n",p->name, p->age);
    close( shmid);
    return 0;
}