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

int main()
{
    shm_unlink("/xyz");
    return 0;
}