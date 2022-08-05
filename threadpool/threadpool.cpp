//
// Created by chengzi on 2022/8/3.
//
// p41 线程池
#include "threadpool.h"
#include <pthread.h>
#include <memory>
#include <iostream>

using namespace std;

void *thread_routine (void *arg)
{
    struct timespec abstime;
    int timeout;

    printf("thread 0x%x is starting\n", (int)pthread_self());
    threadpool_t *pool = (threadpool_t*)arg;
    while (1)
    {
        timeout = 0;
        condition_lock(&pool->ready);
        //等待队列中有任务到来或者线程池销毁通知
        pool->idle++;
        while (pool->first == NULL && !pool->quit)
        {
            printf("thread 0x%x is waiting\n",(int)pthread_self());
            clock_gettime(CLOCK_REALTIME,&abstime);
            abstime.tv_sec += 2;
            //condition_wait(&pool->ready);
            int status = condition_timewait(&pool->ready, &abstime);
            if (status == ETIMEDOUT)
            {
                printf("thread 0x%x is wait timed out\n",(int)pthread_self());
                timeout = 1;
                break;
            }
        }
        //等待到条件，处于工作状态或者超时
        pool->idle--;

        //等待到任务
        if (pool->first != NULL)
        {
            //从队头取出任务
            task_t *t = pool->first;
            pool->first = t->next;
            //执行任务需要一定的时间，所以要先解锁，以便其他生产者进程
            //能够往队列中添加任务，其他消费者线程能够进入等待任务
            condition_unlock(&pool->ready);
            t->run(t->arg);
            free(t);
            condition_lock(&pool->ready);
        }
        //如果等待到线程销毁通知,并且任务都执行完成
        if (pool->quit && pool->first == NULL)
        {
            pool->counter--;
            if (pool->counter == 0) //告诉调用销毁函数的线程能够结束
            {
                condition_signal(&pool->ready);
            }
            //跳出循环之前不要忘了解锁
            condition_unlock(&pool->ready);
            break;
        }
        if (timeout && pool->first == NULL)
        {
            pool->counter--;
            //跳出循环之前不要忘了解锁
            condition_unlock(&pool->ready);
            break;
        }
        condition_unlock(&pool->ready);
    }

    printf("thread 0x%d is exiting\n",(int)pthread_self());
    return NULL;
}

// 初始化线程池
void threadpool_init(threadpool_t *pool, int threads)
{
    //对线程池中的各个字段初始化
    condition_init(&pool->ready);
    pool->first = NULL;
    pool->last = NULL;
    pool->counter = 0;
    pool->idle = 0;
    pool->max_threads = threads;
    pool->quit = 0;
}

// 在线程池中添加任务
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg)
{
    // 生成新任务
    task_t* newtask = (task_t *)malloc(sizeof(task_t));
    newtask->run = run;
    newtask->arg = arg;
    newtask->next = NULL;


    condition_lock(&pool->ready);
    // 将任务添加到队列中
    if (pool->first == NULL)
        pool->first = newtask;
    else
        pool->last->next = newtask;
    pool->last = newtask;

    // 如果有等待任务的线程，则唤醒其中一个
    if (pool->idle > 0)
    {
        condition_signal(&pool->ready);
    }
    else if (pool->counter < pool->max_threads)
    {
        // 没有等待线程，并且当前线程数不超过最大线程数,则创建一个新线程
        pthread_t tid;
        pthread_create(&tid, NULL, thread_routine, pool);
        ++pool->counter;
    }
    condition_unlock(&pool->ready);
}

// 销毁线程池
void threadpool_destroy(threadpool_t *pool)
{
    if (pool->quit)
    {
        return;
    }
    condition_lock(&pool->ready);
    pool->quit = 1;
    if (pool->counter > 0)
    {
        //对处于等待中的线程发起广播
        if (pool->idle > 0)
        {
            condition_broadcast(&pool->ready);
        }
        //处于执行任务中的线程不会受到广播
        //线程池需要等待执行任务状态中的线程全部退出
        while (pool->counter > 0)
        {
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);
    condition_destroy(&pool->ready);
}