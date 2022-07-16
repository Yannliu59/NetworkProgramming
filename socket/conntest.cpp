#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
    Created by lyj on 2022/7/11
    p15 压力测试
 */

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    }while(0)

int main(void)
{
    int cnt = 0;
    while(1) {
        /*
         * 创建一个套接字类型
         * */
        int sock;
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
            sleep(4);
            // 三个参数表示 网际协议，流式套接字，TCP协议
            ERR_EXIT("socket");//创建成功返回0 失败返回-1
        }

        /*
         * 创建并初始化一个ipv4地址结构
         * */
        struct sockaddr_in servaddr; //创建一个ipv4的地址结构
        memset(&servaddr, 0, sizeof servaddr);//初始化地址
        servaddr.sin_family = PF_INET;//初始化地址族为PF_INET 表示IPV4的套接字类型
        servaddr.sin_port = htons(30000);//初始化端口号 需要网络字节序类型
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        /*
         * 连接至服务器端
         * 连接成功 则sock这个套接字处于通信状态
         * */
        if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
            ERR_EXIT("connect");

        struct sockaddr_in localaddr;
        socklen_t addrlen = sizeof(localaddr);
        if (getsockname(sock, (struct sockaddr *) &localaddr, &addrlen) < 0)
            ERR_EXIT("getsockname");
        printf("ip=%s port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
        printf("%d\n",++cnt);
    //    if (getpeername(sock, (struct sockaddr *) &localaddr, &addrlen) < 0)
    //        ERR_EXIT("getpeername");

    //    printf("peerip=%s port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
    }

    return 0;
}
