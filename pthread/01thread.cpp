//
// Created by chengzi on 2022/7/28.
//
// p37 posix线程

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mutex>
#include <pthread.h>

// perror() 检查的是全局的errno这个变量
#define ERR_EXIT(m) \
            do \
            {       \
                perror(m);        \
                exit(EXIT_FAILURE);    \
            } while(0);

void *thread_routine(void *arg)
{
    for (int i = 0; i < 20; i++)
    {
        printf("B");
        fflush(stdout);
        usleep(10);
        if (i == 3){
            pthread_exit((void*)"ABC");
        }
    }
    return (void*) "DEF";
}

int main(void)
{
    pthread_t tid;
    int ret;
    ret = pthread_create(&tid, NULL, thread_routine, NULL);
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create:%s\n",strerror(ret));
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 20; i++)
    {
        printf("A");
        fflush(stdout);
        usleep(10);
    }
    void *value;
    if ((ret = pthread_join(tid, &value)) != 0)
    {
        fprintf(stderr, "pthread_join:%s\n", strerror(ret));
    }
    printf("\n");
    printf("return msg = %s\n", (char*)value);
    return 0;
}