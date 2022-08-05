//
// Created by chengzi on 2022/7/22.
//
// p30  System V信号量（一）

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>

#define ERR_EXIT(m) \
            do \
            {       \
                perror(m);    \
                exit(EXIT_FAILURE);        \
            } while(0);
union semun
{
    int val;                    //Value for SETVAL
    struct semid_ds *buf;       //Buffer for IPC_STAT, IPC_SET
    unsigned short *array;      //Array for GETALL, SETALL ()
    struct seminfo *__buf;      //Buffer for IPC_INFO (linux-specific)
};

int sem_create(key_t key)//创建信号量集
{
    int semid;
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1)
    {
        ERR_EXIT("semget");
    }
    return semid;
}

int sem_open(key_t key)//打开信号量集
{
    int semid;
    semid = semget(key, 0, 0);
    if (semid == -1)
    {
        ERR_EXIT("semid");
    }
    return semid;
}

int sem_setval(int semid, int val)//设置信号量集
{
    union semun su;
    su.val = val;
    int ret;
    ret = semctl(semid, 0, SETVAL, su);
    if (ret == -1)
        ERR_EXIT("sem setval");
    return 0;
}

int sem_getval(int semid)//获得信号量的值
{
    int ret;
    ret = semctl(semid, 0, GETVAL, 0);
    if (ret == -1)
        ERR_EXIT("gem setval");
    return ret;
}

int sem_del(int semid)//删除信号量集
{
    int ret;
    semctl(semid, 0, IPC_RMID, 0);
    if (ret == -1)
        ERR_EXIT("semctl");
    return 0;
}

int sem_p(int semid)//  P操作
{
    struct sembuf sb = {0, -1, 0};
    int ret;
    ret = semop(semid, &sb, 1);
    if(ret == -1)
    {
        ERR_EXIT("semop");
    }
    return ret;
}

int sem_v(int semid)//  V操作
{
    struct sembuf sb = {0, 1, 0};
    int ret;
    ret = semop(semid, &sb, 1);
    if(ret == -1)
    {
        ERR_EXIT("semop");
    }
    return ret;
}

int main()
{
    int semid;
    semid = sem_create(1234);
    sleep(20);
    sem_del(semid);
    return 0;

}