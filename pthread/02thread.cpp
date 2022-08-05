//
// Created by chengzi on 2022/7/28.
//
// p38 posix线程(二)

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

int main(void)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int state;
    pthread_attr_getdetachstate(&attr,&state);
    if (state == PTHREAD_CREATE_JOINABLE)
        printf("detachstate:PTHREAD_CREATE_JOINABLE\n");
    else if (state == PTHREAD_CREATE_DETACHED)
        printf("detachstate:PTHREAD_CREATE_DETACHED\n");

    size_t size;
    pthread_attr_getstacksize(&attr, &size);
    printf("stacksize:%d\n",size);

    pthread_attr_getguardsize(&attr, &size);
    printf("guardsize:%d\n",size);

    int scope;
    pthread_attr_getscope(&attr, &scope);
    if (scope == PTHREAD_SCOPE_PROCESS)
        printf("scope:PTHREAD_SCOPE_PRECESS\n");
    if (scope == PTHREAD_SCOPE_SYSTEM)
        printf("scope:PTHREAD_SCOPE_SYSTEM\n");


    return 0;
}