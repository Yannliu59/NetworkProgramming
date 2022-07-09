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

using namespace std;

/*
    Created by lyj on 2022/7/10.

    p16 套接字IO超时设置方法
        用select实现超时
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


    /*
     * accept函数从已完成连接队列返回第一个连接，如果已完成连接队列为空，则阻塞
     * listenfd：服务器套接字
     * peeraddr：将返回对等方的套接字地址
     * peerlen：将返回对等方的套接字地址长度
     * */
    struct sockaddr_in peeraddr;//定义一个对等方的地址
    socklen_t peerlen;
    int conn;//接受accept返回的连接的描述符

    int client[FD_SETSIZE];
    int maxi = 0;
    for(int i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;

    int nready;
    int maxfd = listenfd;
    fd_set rset;
    fd_set allset;
    FD_ZERO(&rset);
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);

    while(1){
        int i;
        rset = allset;
        nready = select(maxfd + 1,&rset,NULL,NULL,NULL);
        if(nready == -1){
            if(errno == EINTR)
                continue;
            ERR_EXIT("select");
        }
        if (nready == 0)continue;

        if(FD_ISSET(listenfd,&rset)){ //处理监听套接口
            peerlen = sizeof(peeraddr);
            conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen);

            if(conn == -1)
                ERR_EXIT("select");

            for(i = 0; i < FD_SETSIZE; i++){
                if(client[i] < 0){
                    client[i] = conn;
                    maxi = max(maxi,i);
                    break;
                }
            }
            if(i == FD_SETSIZE){
                fprintf(stderr,"too many clients\n");
                exit(EXIT_FAILURE);
            }
            printf("ip = %s port = %d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
            FD_SET(conn,&allset);
            maxfd = max(conn,maxfd);
            if(--nready <=0)
                continue;
        }

        for(i = 0; i <= maxi; i++){ //处理其他连接套接口
            conn = client[i];
            if(conn == -1)
                continue;
            if(FD_ISSET(conn,&rset)){
                char recvbuf[1024] = {0};
                int ret = readline(conn,recvbuf,1024);
                if(ret == -1)
                    ERR_EXIT("readline");
                if(ret == 0){
                    printf("client close\n");
                    FD_CLR(conn,&allset);
                    client[i] = -1;
                    close(conn);
                }
                fputs(recvbuf,stdout);
                sleep(4);
                writen(conn,recvbuf,strlen(recvbuf));
                if(--nready <= 0)
                    break;
            }
        }



    }
    return 0;
}
