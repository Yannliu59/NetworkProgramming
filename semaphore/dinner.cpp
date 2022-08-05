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
#include <ctime>
#include <random>

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


int semid;
std::mt19937 srd(getpid());
#define DELAY srd() % 6

void wait_for_2fork(int no)
{
     int left = no;
     int right = (no + 1) % 5;
     struct sembuf buf[2] = {
             {static_cast<unsigned short>(left), -1, 0},
             {static_cast<unsigned short>(right), -1, 0}
     };
     semop(semid, buf, 2);
}

void free_2fork(int no)
{
    int left = no;
    int right = (no + 1) % 5;
    struct sembuf buf[2] = {
            {static_cast<unsigned short>(left), 1, 0},
            {static_cast<unsigned short>(right), 1, 0}
    };
    semop(semid, buf, 2);
}

void philosopher(int no)
{
    for (;;)
    {
        printf("%d is thinking\n",no);
        sleep(DELAY);
        printf("%d is hungry\n",no);
        wait_for_2fork(no);
        printf("%d is eating\n", no);
        sleep(DELAY);
        free_2fork(no);
    }
}

int main()
{
    semid = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);
    if (semid == -1)
    {
        ERR_EXIT("semget");
    }

    int i;
    union semun su;
    su.val = 1;
    for (i = 0; i < 5; i++)
    {
        semctl(semid, i, SETVAL, su);
    }

    int no = 0;
    pid_t pid;
    for (i = 1; i < 5; i++)
    {
        pid = fork();
        if (pid == -1)
            ERR_EXIT("fork");
        if (pid == 0)
        {
            no = i;
            break;
        }
    }
    philosopher(no);
    return 0;
}