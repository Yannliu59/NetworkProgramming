//
// Created by chengzi on 2022/7/16.
//

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <error.h>
#include <cstdlib>
#include <cstdio>

#define ERR_EXIT(m) \
            do \
            {       \
                perror(m);        \
                exit(EXIT_FAILURE);    \
            } while(0);

void echosrv(int conn);

int main()
{
    int listenfd;
    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0))< 0)
        ERR_EXIT("socket");

    unlink("test_socket");
    struct sockaddr_un servaddr;
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, "test_socket");

    if (bind(listenfd,(struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");

    if (listen(listenfd, SOMAXCONN) < 0)
        ERR_EXIT("listen");

    int conn;
    pid_t pid;

    while (1)
    {
        conn = accept(listenfd, NULL, NULL);
        if (conn == -1)
        {
            if (conn == EINTR)
                continue;
            ERR_EXIT("accept");
        }

        pid = fork();
        if (pid == -1)
            ERR_EXIT("fork");
        if (pid == 0)
        {
            close(listenfd);//子进程不需要处理监听
            echosrv(conn);
            exit(EXIT_SUCCESS);
        }
        close(conn);//父进程不需要处理连接
    }


    return 0;
}

void echosrv(int conn)
{
    char recvbuf[1024];
    int n;
    while (1)
    {
        memset(recvbuf, 0, sizeof recvbuf);
        n = read(conn, recvbuf, sizeof(recvbuf));
        if (n == -1)
        {
            if (n == EINTR)
                continue;
            ERR_EXIT("read");
        }
        else if (n == 0)
        {
            printf("client close\n");
            break;
        }
        fputs(recvbuf, stdout);
        write(conn, recvbuf, strlen(recvbuf));
    }
    close(conn);
}
