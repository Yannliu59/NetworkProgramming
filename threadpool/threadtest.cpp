//
// Created by chengzi on 2022/8/3.
//
#include "condition.h"
#include "threadpool.h"
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

void *mytask (void *arg)
{
    printf("thread 0x%x working task %d\n", (int)pthread_self(), *(int*)(arg));
    sleep(1);
    free(arg);
    return NULL;
}

int main(void)
{
    threadpool_t pool;
    threadpool_init(&pool, 3);

    for (int i = 0; i < 10; ++i)
    {
        int *arg = (int*)malloc(sizeof(int));
        *arg = i;
        threadpool_add_task(&pool, mytask, arg);
    }
    threadpool_destroy(&pool);
    return 0;
}