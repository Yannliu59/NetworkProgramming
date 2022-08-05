//
// Created by chengzi on 2022/7/19.
//
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#define ERR_EXIT(m) \
        do \
        {           \
            perror(m);            \
            exit(EXIT_FAILURE);        \
        } while(0);
#define MSGMAX 8192

int main(int argc, char *argv[])
{
    int flag = 0;
    long type = 0;
    while (1)
    {
        //参数n和参数t :表示能再跟一个参数，保存在optarg中
        int opt = getopt(argc, argv, "nt:");
        if (opt == '?')
            exit(EXIT_FAILURE);
        if (opt == -1)
        {
            break;
        }
        switch (opt)
        {
            case 'n':
//                printf("AAAA\n");
                flag |= IPC_NOWAIT;
                break;
            case 't':
//                printf("BBBB\n");
//                int n = atoi(optarg);
//                printf("n = %d\n", n);
                type = atoi(optarg);
                break;
        }
    }
    int msgid;
    msgid = msgget(1234,0);
    if (msgid == -1)
        ERR_EXIT("msgget");
    struct msgbuf *ptr;
    ptr = (struct msgbuf*)malloc(sizeof(long) + MSGMAX);
    ptr -> mtype = type;
    int nrcv = 0;
    if ((nrcv = msgrcv(msgid, ptr, MSGMAX, type, flag))< 0)
        ERR_EXIT("msgsnd");
    printf("read %d bytes type = %ld\n", nrcv, ptr->mtype);
    return 0;
}