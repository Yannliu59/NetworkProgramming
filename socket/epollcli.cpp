#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <fcntl.h>

/*
    Created by lyj on 2022/7/11.

    p17 epoll
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

void echo_cli(int sock)
{
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};

    fd_set rset;
    FD_ZERO(&rset);

    int maxfd;
    int nready;
    int fd_stdin = fileno(stdin);
    maxfd = std::max(fd_stdin,sock);

    int stdineof = 0;

    while(1){
        if(stdineof == 0)
            FD_SET(fd_stdin,&rset);
        FD_SET(sock,&rset);
        nready = select(maxfd + 1,&rset,NULL,NULL,NULL);
        if(nready == -1)
            ERR_EXIT("select");
        if(nready == 0)
            continue;
        if (FD_ISSET(sock,&rset)){
            int ret = readline(sock,recvbuf,sizeof(recvbuf));
            if(ret == -1)
                ERR_EXIT("readline");
            else if(ret == 0){
                printf("sever close\n");
                break;
            }
            fputs(recvbuf,stdout);
            memset(recvbuf,0,sizeof recvbuf);
        } else if(FD_ISSET(fd_stdin,&rset)){
            if(fgets(sendbuf,sizeof(sendbuf),stdin) == NULL)
            {
                stdineof = 1;
                /*
                close(sock);
                */
                shutdown(sock,SHUT_WR);

            } else {
                writen(sock,sendbuf,strlen(sendbuf));
                memset(sendbuf,0,sizeof sendbuf);
            }
        }
    }
    close(sock);
}

void handle_sigpipe(int sig)
{
    printf("recv a sig = %d\n",sig);
}

void activate_nonblock(int fd)//设置文件描述符为非阻塞模式
{
    int ret;
    int flags = fcntl(fd,F_GETFL);//先获取文件的标志
    if (flags == -1)
        ERR_EXIT("fcntl");
    flags |= SOCK_NONBLOCK;//添加非阻塞模式
    ret = fcntl(fd,F_SETFL,flags);//设置
    if(ret == -1)
        ERR_EXIT("fcntl");
}

void deactivate_nonblock(int fd)//设置文件描述符为阻塞模式
{
    int ret;
    int flags = fcntl(fd,F_GETFL);//先获取文件的标志
    if (flags == -1)
        ERR_EXIT("fcntl");
    flags &= SOCK_NONBLOCK;//添加非阻塞模式
    ret = fcntl(fd,F_SETFL,flags);//设置
    if(ret == -1)
        ERR_EXIT("fcntl");
}

int connect_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if(wait_seconds > 0)
        activate_nonblock(fd);

    ret = connect(fd,(struct sockaddr*)addr,addrlen);
    if (ret < 0 && errno == EINPROGRESS) //连接正在处理当中
    {
        fd_set connect_fdset;
        struct timeval timeout;
        FD_ZERO(&connect_fdset);
        FD_SET(fd,&connect_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do{
            /* 一旦连接建立，套接字就可写 */
            ret = select(fd + 1,NULL,&connect_fdset,NULL,&timeout);
        }while(ret < 0 && errno == EINTR);
        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret < 0)
            return -1;
        else if (ret == 1)
        {
            /* ret返回为1 可能有两种情况，一种是连接建立成功，一种是套接字产生错误 */
            /* 此时错误信息不会保存至errno变量中，因此，需要调用getsickopt获取 */
            int err;
            socklen_t socklen = sizeof(err);
            int sockoptret = getsockopt(fd,SOL_SOCKET,SO_ERROR,&err,&socklen);
            if (sockoptret == -1)
            {
                return -1;
            }
            if (err == 0)
                ret = 0;
            else
            {
                errno = err;
                ret = -1;
            }
        }
    }
    if (wait_seconds > 0)
    {
        deactivate_nonblock(fd);
    }
    return ret;
}


int main(void)
{
    /*
     * 创建一个套接字类型
     * */
    signal(SIGPIPE,handle_sigpipe);

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
    /*    if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
        ERR_EXIT("connect");
    */
    int ret = connect_timeout(sock,&servaddr,5);
    if(ret == -1 && errno == ETIMEDOUT)
    {
        printf("timeout......\n");
        return 1;
    }
    else if(ret == -1)
        ERR_EXIT("connect_timeout\n");

    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if(getsockname(sock,(struct sockaddr*)&localaddr,&addrlen) < 0)
        ERR_EXIT("getsockname");

    printf("ip=%s port=%d\n",inet_ntoa(localaddr.sin_addr),ntohs(localaddr.sin_port));

    if(getpeername(sock,(struct sockaddr*)&localaddr,&addrlen) < 0)
        ERR_EXIT("getpeername");

    printf("peerip=%s port=%d\n",inet_ntoa(localaddr.sin_addr),ntohs(localaddr.sin_port));

    echo_cli(sock);
    return 0;
}
