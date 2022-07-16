#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * 2022/7/12
 * p19 UDP实现回射客户服务器
 * */

#define ERR_EXIT(m) \
        do{                 \
          perror(m);          \
          exit(EXIT_FAILURE);  \
        }while(0)

void echo_srv(int sock)
{
    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;

    while (1)
    {
        peerlen = sizeof(peeraddr);
        memset(recvbuf,0,sizeof recvbuf);
        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&peeraddr, &peerlen);
        if (n == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }
        else if (n > 0)
        {
            fputs(recvbuf,stdout);
            sendto(sock, recvbuf, n, 0, (struct sockaddr*)&peeraddr, peerlen);
        }
    }
    close(sock);
}

void echo_cli(int sock)
{
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = ntohs(30000);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ((connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr))) < 0)
        ERR_EXIT("connect");

    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    int ret = 0;
    while (fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
    {
        /*
         * 客户端地址不需要绑定，在第一次sendto的时候就会绑定一个地址
         * */
        sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
        ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);

        if (ret == -1)//检测异步错误
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("recbfrom");
        }

        fputs(sendbuf, stdout);
        memset(sendbuf, 0, sizeof sendbuf);
        memset(recvbuf, 0, sizeof recvbuf);
    }
    close(sock);
}

int main()
{
    /*
     * 对于UDP来说，不需要监听函数，因为不需要三次握手
     * */

    int sock;
    if ((sock = socket(PF_INET,SOCK_DGRAM,0)) < 0)
        ERR_EXIT("socket");

    echo_cli(sock);
    return 0;
}