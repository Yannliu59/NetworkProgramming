//
// Created by chengzi on 2022/7/20.
//
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <cstring>

#define ERR_EXIT(m) \
        do \
        { \
             perror(m); \
             exit(EXIT_FAILURE);    \
        } while (0);
#define MSGMAX 8192

void echo_cli(int msgid)
{
    int n;
    int pid;
    pid = getppid();
    struct msgbuf msg;
    memset(& msg, 0, sizeof msg);
    *((int*)msg.mtext) = pid;
    msg.mtype = 1;
    while (fgets(msg.mtext + 4, MSGMAX, stdin) != NULL)
    {
        if (msgsnd(msgid, &msg, 4 + strlen(msg.mtext + 4), 0) < 0)
        {
            ERR_EXIT("msgsnd");
        }
        memset(msg.mtext + 4, 0, MSGMAX - 4);
        if ((n = msgrcv(msgid, &msg, MSGMAX, pid, 0)) < 0)
        {
            ERR_EXIT("msgsnd");
        }
        fputs(msg.mtext + 4,stdout);
        memset(msg.mtext + 4, 0, MSGMAX - 4);
    }
}

int main()
{
    int msgid;
    msgid = msgget(1234, 0);
    if (msgid == -1)
        ERR_EXIT("msgget");

    echo_cli(msgid);
    return 0;
}