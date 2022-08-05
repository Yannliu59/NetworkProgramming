//
// Created by chengzi on 2022/7/19.
//
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <cstdlib>
#include <stdio.h>
#include <errno.h>

#define ERR_EXIT(m) \
        do \
        {           \
            perror(m);            \
            exit(EXIT_FAILURE);        \
        } while(0)

int main()
{
    int msgid;

    //创建消息队列
//    msgid = msgget(1234, 0666 | IPC_CREAT);
//  msgid = msgget(1234, 0666 | IPC_CREAT | IPC_EXCL);
//  msgid = msgget(IPC_PRIVATE, 0666)

    //打开消息队列
    msgid = msgget(1234,0);
    if (msgid == -1)
        ERR_EXIT("msgget");

    printf("msgid succ\n");
    printf("msgid = %d\n", msgid);

    //控制消息队列
//  msgctl(msgid, IPC_RMID,NULL); //删除消息队列

    struct msqid_ds buf;
    msgctl(msgid, IPC_STAT, &buf); //获取消息队列状态
    printf("mode = %o\n", buf.msg_perm.mode);
    printf("bytes = %ld\n", buf.__msg_cbytes);
    printf("number=%d\n", (int)buf.msg_qnum);
    printf("msgmnb=%d\n", (int)buf.msg_qbytes);

//    sscanf("600", "%o", (unsigned int*)&buf.msg_perm.mode);
//    msgctl(msgid, IPC_SET, &buf);

    return 0;
}