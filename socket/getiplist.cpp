#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

/*
    Created by lyj on 2022/7/1.
    p10 socket编程
    获取主机名，ip等信息
 */

#define ERR_EXIT(m) \
        do{ \
            perror(m); \
            exit(EXIT_FAILURE); \
        }while(0);

int getlocalip(char *ip)
{
    char host[100] = {0};
    if(gethostname(host,sizeof(host)) < 0)
        return -1;

    struct hostent* hp;
    if((hp = gethostbyname(host)) == NULL)
        return -1;
    strcpy(ip,inet_ntoa(*(struct in_addr*)hp->h_addr_list[0]));
}

int main()
{
    char host[100] = {0};
    if (gethostname(host,sizeof(host)) < 0)
        ERR_EXIT("gethostname");

    struct hostent *hp;
    if((hp = gethostbyname(host)) == NULL)
        ERR_EXIT("gethostbyname");
    int i = 0;
    while (hp->h_addr_list[i] != NULL){
        printf("%s\n",inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
        i++;
    }
    char ip[16] = {0};
    getlocalip(ip);
    printf("localip = %s",ip);
    return 0;
}