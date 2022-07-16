#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <wait.h>
#include <poll.h>
#include <vector>
#include <algorithm>
#include <sys/epoll.h>
#include <fcntl.h>

using namespace std;

/*
    Created by lyj on 2022/7/11

    p18 epoll
 */

#define ERR_EXIT(m) \
        do{ \
            perror(m); \
            exit(EXIT_FAILURE); \
        }while(0)

struct packet{
    int len;
    char buf[1024];
};

void deactivate_nonblock(int fd);
void activate_nonblock(int fd);
ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd,void *buf,size_t len);
ssize_t readline(int sockfd,void *buf,size_t maxline);
void echo_service(int conn);
void echo_service2(int conn);
void handle_sigpipe(int sig);
void handle_sigchld(int sig);

typedef vector<struct epoll_event> EventList;


int main(void)
{
    //signal(SIGCHLD,SIG_IGN);
    signal(SIGPIPE,handle_sigpipe);
    signal(SIGCHLD,handle_sigchld);

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


    std::vector<int> clients; //连接的套接字数
    int epollfd;
    epollfd = epoll_create1(EPOLL_CLOEXEC); //注册epoll事件

    struct epoll_event event;
    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event);

    EventList events(16);
    struct sockaddr_in peeraddr;
    socklen_t peerlen;

    int conn;
    int i;
    int nready;
    int count = 0;

    while (1)
    {
        nready = epoll_wait(epollfd,&*events.begin(),static_cast<int>(events.size()),-1);
        if (nready == -1)
        {
            if(errno == EINTR)
                continue;
            ERR_EXIT("epoll_wait");
        }
        if (nready == 0)
            continue;
        if ((size_t)nready == events.size())    //容器数量不够多
            events.resize(events.size() * 2);

        for (i = 0; i < nready; i++){
            if (events[i].data.fd == listenfd)
            {
                peerlen = sizeof(peeraddr);
                conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen);
                if(conn == -1)
                    ERR_EXIT("accept");
                printf("ip = %s port = %d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
                printf("count = %d\n",++count);
                clients.push_back(conn);
                activate_nonblock(conn);  //因为是边缘触发模式

                event.data.fd = conn;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,conn,&event);
            }
            else if (events[i].events & EPOLLIN)
            {
                conn = events[i].data.fd;
                if (conn < 0)
                    continue;
                char recvbuf[1024] = {0};
                int ret = readline(conn,recvbuf,1024);
                if (ret == -1)
                    ERR_EXIT("readline");
                if (ret == 0)
                {
                    printf("client close\n");
                    close(conn);
                    event = events[i];
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,conn,&event);
                    clients.erase(std::remove(clients.begin(),clients.end(),conn),clients.end());
                }
                fputs(recvbuf,stdout);
                writen(conn,recvbuf,strlen(recvbuf));
            }
        }
    }


    return 0;
}

void activate_nonblock(int fd)//设置文件描述符为非阻塞模式
{
    int ret;
    int flags = fcntl(fd,F_GETFL);//先获取文件的标志
    if (flags == -1)
        ERR_EXIT("fcntl");
    flags |= O_NONBLOCK;//添加非阻塞模式
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
    flags &= O_NONBLOCK;//添加非阻塞模式
    ret = fcntl(fd,F_SETFL,flags);//设置
    if(ret == -1)
        ERR_EXIT("fcntl");
}



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

void echo_service(int conn){//借助readn读取定长数据解决粘包
    struct packet recvbuf;
    int n;
    while(1){
        memset(&recvbuf,0,sizeof recvbuf);
        int ret = readn(conn,&recvbuf.len,4);
        if(ret == -1)
            ERR_EXIT("read");
        else if(ret < 4){
            printf("client_close\n");
            break;
        }
        n = ntohl(recvbuf.len);
        ret = readn(conn,&recvbuf.buf,n);
        if(ret == -1)
            ERR_EXIT("read");
        else if(ret < n){
            printf("client_close\n");
            break;
        }
        fputs(recvbuf.buf,stdout);
        writen(conn,&recvbuf,4 + n);
    }
}

void echo_service2(int conn){//借助readline读取一行来解决粘包
    char recvbuf[1024];
    int n;
    while(1){
        memset(recvbuf,0,sizeof recvbuf);
        int ret = readline(conn,recvbuf,1024);
        if(ret == -1)
            ERR_EXIT("readline");
        if(ret == 0){
            printf("client close\n");
            break;
        }
        fputs(recvbuf,stdout);
        writen(conn,recvbuf,strlen(recvbuf));
    }
}

void handle_sigchld(int sig)
{
    while(waitpid(-1,NULL,WNOHANG) > 0);
}
void handle_sigpipe(int sig)
{
    printf("sig = %d\n",sig);
}