#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
    Created by lyj on 2022/7/1.
    p7 socket编程
    实现一个回射服务器客户端
    p9 socket编程：解决粘包问题
    1.通过发收定长包解决粘包问题 缺点：增加网络负担
    2.自定义协议包
 */

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    }while(0)

struct packet{
    int len;
    char buf[1024];
};

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char*)buf;
    while(nleft > 0){
        if ((nread = read(fd,bufp,nleft)) < 0){
            if(errno == EINTR)//信号中断情况
                continue;
            return -1;//失败返回-1
        } else if(nread == 0)
            return count - nleft;//读到了EOF
        bufp += nread;
        nleft -= nread;
    }
    return count;
}

ssize_t writen(int fd, const void *buf, size_t count){
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char*)buf;
    while(nleft > 0){
        if ((nwritten = write(fd,bufp,nleft)) < 0){
            if(errno == EINTR)//信号中断情况
                continue;
            return -1;//失败返回-1
        } else if(nwritten == 0)
            continue;
        bufp += nwritten;
        nleft -= nwritten;
    }
    return count;

}


int main(void)
{
    /*
     * 创建一个套接字类型
     * */
    int sock;
    if((sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)// 三个参数表示 网际协议，流式套接字，TCP协议
        ERR_EXIT("socket");//创建成功返回0 失败返回-1


    /*
     * 创建并初始化一个ipv4地址结构
     * */
    struct sockaddr_in servaddr; //创建一个ipv4的地址结构
    memset(&servaddr,0,sizeof servaddr);//初始化地址
    servaddr.sin_family = PF_INET;//初始化地址族为PF_INET 表示IPV4的套接字类型
    servaddr.sin_port = htons(30000);//初始化端口号 需要网络字节序类型
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /*
     * 连接至服务器端
     * 连接成功 则sock这个套接字处于通信状态
     * */
    if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
        ERR_EXIT("connect");

    struct packet sendbuf;
    struct packet recvbuf;
    memset(&sendbuf,0,sizeof sendbuf);
    memset(&recvbuf,0,sizeof recvbuf);

    int n;
    while (fgets(sendbuf.buf,sizeof(sendbuf.buf),stdin) != NULL){
        n = strlen(sendbuf.buf);
        sendbuf.len = htonl(n);
        writen(sock,&sendbuf,4 + n);

        int ret = readn(sock,&recvbuf.len,4);
        if(ret == -1)
            ERR_EXIT("read");
        else if(ret < 4){
            printf("client_close\n");
            break;
        }
        n = ntohl(recvbuf.len);
        ret = readn(sock,&recvbuf.buf,n);
        if(ret == -1)
            ERR_EXIT("read");
        else if(ret < n){
            printf("client_close\n");
            break;
        }

        fputs(recvbuf.buf,stdout);
        memset(&recvbuf,0,sizeof recvbuf);
        memset(&sendbuf,0,sizeof sendbuf);
    }
    close(sock);
    return 0;
}
