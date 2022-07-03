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
ssize_t recv_peek(int sockfd,void *buf,size_t len)
{
    /*
     * recv函数只能用于套接口IO,read函数能用于套接口IO和文件IO
     * MSG_PEEK：从套接口缓冲区中接受数据到buf当中
     *           并没有从套接口缓冲区中将数据移除
     * */
    while(1){
        int ret = recv(sockfd,buf,len,MSG_PEEK);
        if (ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}

ssize_t readline(int sockfd,void *buf,size_t maxline)
{
    /*
     * 读取一行消息
     * */
    int ret;
    int nread;
    char *bufp = (char*)buf;
    int nleft = maxline;
    while(1){
        ret = recv_peek(sockfd,bufp,nleft);
        if(ret < 0)
            return ret;
        else if(ret == 0)
            return ret;
        nread = ret;
        int i;
        for(int i = 0; i < nread; i++){
            if(bufp[i] == '\n'){
                ret = readn(sockfd,bufp,i + 1);
                if(ret != i + 1)
                    exit(EXIT_FAILURE);
                return ret;
            }
        }
        if(nread > nleft)
            exit(EXIT_SUCCESS);
        nleft -= nread;
        ret = readn(sockfd,bufp,nread);
        if(ret != nread)
            exit(EXIT_FAILURE);
        bufp += nread;
    }
    return -1;
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

    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if(getsockname(sock,(struct sockaddr*)&localaddr,&addrlen) < 0)
        ERR_EXIT("getsockname");

    printf("ip=%s port=%d\n",inet_ntoa(localaddr.sin_addr),ntohs(localaddr.sin_port));

    if(getpeername(sock,(struct sockaddr*)&localaddr,&addrlen) < 0)
        ERR_EXIT("getpeername");

    printf("peerip=%s port=%d\n",inet_ntoa(localaddr.sin_addr),ntohs(localaddr.sin_port));

    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while (fgets(sendbuf,sizeof(sendbuf),stdin) != NULL){

        writen(sock,sendbuf,strlen(sendbuf));

        int ret = readline(sock,recvbuf,sizeof recvbuf);
        if(ret == -1)
            ERR_EXIT("readline");
        else if(ret == 0){
            printf("client_close\n");
            break;
        }

        fputs(recvbuf,stdout);
        memset(recvbuf,0,sizeof recvbuf);
        memset(sendbuf,0,sizeof sendbuf);
    }
    close(sock);
    return 0;
}
