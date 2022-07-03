#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

/*
    Created by lyj on 2022/7/2
    p8 socket编程
    实现一个回射服务器的服务端 并且能够接受多个客户端请求
    多进程方式 实现点对点的聊天
 */

#define ERR_EXIT(m) \
        do{ \
            perror(m); \
            exit(EXIT_FAILURE); \
        }while(0);

void do_service(int conn){//子进程处理通信
    char recvbuf[1024];
    while(1){
        memset(recvbuf,0,sizeof recvbuf);
        int ret = read(conn,recvbuf,sizeof recvbuf);

        if(ret == 0){
            printf("client_close\n");
            break;
        } else if(ret == -1)
            ERR_EXIT("read");

        fputs(recvbuf,stdout);
        write(conn,recvbuf,ret);
    }
}


int main(void)
{
    /*
     * 创建一个套接字类型
     * */
    int listenfd;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        ERR_EXIT("socket error");
    }


    /*
     * 创建并初始化一个ipv4地址结构
     * */
    struct sockaddr_in servaddr; //创建一个ipv4的地址结构
    memset(&servaddr,0,sizeof servaddr);//初始化地址
    servaddr.sin_family = PF_INET;//初始化地址族为PF_INET 表示IPV4的套接字类型
    servaddr.sin_port = htons(30000);//初始化端口号 需要网络字节序类型
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);// 初始化ip地址 INADDR_ANY表示本机任意地址


    /*
     * 将套接字类型与ipv4地址结构结构绑定
     * 当服务器端启动时候，此时网络地址处于TIME_WAIT状态,无法绑定
     * 最好开启地址重复利用，调用setsockopt函数
     * */
    int on = 1;
    if((setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))) < 0)
        ERR_EXIT("setsockop");
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)//将套接字与地址绑定
        ERR_EXIT("bind");//创建成功返回0 失败返回-1


    /*
     * 调用listen函数将会使得主动套接字变为被动套接字
     * 主动套接字用于发起连接 调用connect()函数
     * 被动套接字用于接受连接 调用accept()函数
     * 服务器端与客户端的区别主要是服务器端用了两个套接字，监听套接字和已连接套接字
     * 监听套接字主要用来接受三次握手数据，一旦握手完成，就把它放到已连接队列
     * accept就可以从队列返回一个已连接套接字，主要用来通信
     * */
    if(listen(listenfd,SOMAXCONN) < 0)//将套接字用于监听进入的连接 SOMAXCONN表示端口最大监听队列的长度
        ERR_EXIT("listen");//创建成功返回0 失败返回-1


    /*
     * accept函数从已完成连接队列返回第一个连接，如果已完成连接队列为空，则阻塞
     * listenfd：服务器套接字
     * peeraddr：将返回对等方的套接字地址
     * peerlen：将返回对等方的套接字地址长度
     * */
    struct sockaddr_in peeraddr;//定义一个对等方的地址
    socklen_t peerlen = sizeof(peeraddr);
    int conn;//接受accept返回的连接的描述符

    pid_t pid;
    while(1) {
        if ((conn = accept(listenfd, (struct sockaddr *) &peeraddr, &peerlen)) < 0)
            ERR_EXIT("accept");

        printf("ip = %s port = %d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

        pid = fork();
        if(pid == -1)ERR_EXIT("fork");
        if(pid == 0){
            close(listenfd);
            do_service(conn);
            exit(EXIT_SUCCESS);
        } else {
            close(conn);
        }
    }
    return 0;
}
