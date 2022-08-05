//
// Created by chengzi on 2022/7/30.
//
// p39 posix信号量与互斥锁

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// perror() 检查的是全局的errno这个变量
#define ERR_EXIT(m) \
            do \
            {       \
                perror(m);        \
                exit(EXIT_FAILURE);    \
            } while(0);
#define CONSUMER_COUNT 10
#define PRODUCER_COUNT 3

pthread_mutex_t g_mutex;//互斥锁
pthread_cond_t g_cond;//条件变量

pthread_t g_thread[CONSUMER_COUNT + PRODUCER_COUNT];

int nready = 0;

void *consume(void *arg)
{
    int num = *((int*)arg);
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        while (nready == 0)
        {
            printf("%d begin wait a condtion...\n", num);
            pthread_cond_wait(&g_cond, &g_mutex);
        }
        printf("%d end wait a conditon...\n", num);
        --nready;
        pthread_mutex_unlock(&g_mutex);
        sleep(1);
    }
    return NULL;
}

void *produce(void *arg)
{
    int num = *((int*)arg);
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        printf("%d begin produce a product\n", num);
        ++nready;
        printf("%d end produce a product\n", num);
        pthread_cond_signal(&g_cond);
        printf("%d signal\n",num);
        pthread_mutex_unlock(&g_mutex);
        sleep(5);
    }
    return NULL;
}

int main(void)
{
    int i;

    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_cond, NULL);

    for (i = 0; i < CONSUMER_COUNT; i++)
    {
        int *p = (int*)malloc(sizeof(int));
        *p = i;
        pthread_create(&g_thread[i], NULL, consume, (void*)p);
    }
    sleep(1);
    for (i = 0; i < PRODUCER_COUNT; i++)
    {
        pthread_create(&g_thread[CONSUMER_COUNT + i], NULL, produce, &i);
    }


    for (i = 0; i < CONSUMER_COUNT  + PRODUCER_COUNT; i++)
    {
        pthread_join(g_thread[i], NULL);
    }

    pthread_mutex_destroy(&g_mutex);
    pthread_cond_destroy(&g_cond);
    return 0;
}