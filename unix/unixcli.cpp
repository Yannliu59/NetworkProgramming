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

void echocli(int sock);

int main()
{
    int sock;
    if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_un servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path,"test_socket");

    int conn;
    if ((conn = connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        ERR_EXIT("connect");
    }
    echocli(sock);
    return 0;
}

void echocli(int sock)
{
    char sendbuf[1024];
    char recvbuf[1024];
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        write(sock, sendbuf, strlen(sendbuf));
        read(sock, recvbuf, sizeof recvbuf);
        fputs(recvbuf,stdout);
        memset(sendbuf, 0, sizeof sendbuf);
        memset(recvbuf, 0, sizeof recvbuf);
    }
    close(sock);
}

