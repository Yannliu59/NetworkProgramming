//
// Created by chengzi on 2022/7/26.
//
#include <csignal>
#include <sys/mman.h>
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
    shmid = shm_open("/xyz", O_CREAT | O_RDWR, 0666);
    if (shmid == -1)
        ERR_EXIT("shm_open");
    printf("share memory %d create success\n", shmid);

    if (ftruncate(shmid, sizeof(STU)) == -1) //修改共享内存大小
        ERR_EXIT("ftruncate");

    struct stat buf;
    if (fstat(shmid, &buf) == -1)
        ERR_EXIT("fstat");

    printf("size = %ld mode = %o\n", buf.st_size, buf.st_mode & 07777);

    close( shmid);
    return 0;
}