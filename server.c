#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
//socket头文件
#include<arpa/inet.h>
#include<pthread.h>
#include"threadPool.h"

//信息结构体
struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
    /* data */
};

//
typedef struct PoolInfo
{
    ThreadPool *pool;
    int fd;
}PoolInfo;

void working(void*);
void acceptCoon(void*agr);

int main()
{
    //1.创建监听的套接字            fd=socket()
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1)
    {
        perror("socket creat failed.....\n");
        return -1;
    }

    //2.绑定本地的IP port           ret=bind(fd);
    struct sockaddr_in saddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(9999);
    saddr.sin_addr.s_addr=INADDR_ANY;   //0=0.0.0.0
    int ret=bind(fd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret==-1)
    {
        perror("bind creat failed....\n");
        return -1;
    }

    //3.设置监听                        ret=listen(fd)
    ret=listen(fd,128);
     if(ret==-1)
    {
        perror("listen failed....\n");
        return -1;
    }

    //4.创建线程池
    ThreadPool *pool= threadPoolCreate(3,8,100);
    //初始化PoolIfo结构体传递 fd,threadpool 到线程池（放入堆内存）
    PoolInfo *info=(PoolInfo*)malloc(sizeof(PoolInfo));
    info->fd=fd;
    info->pool=pool;
    //给线程池添加监听任务acceptCoon()函数
    threadPoolAdd(pool,acceptCoon,info);
    pthread_exit(NULL);
    return 0;
}

void acceptCoon(void*arg)
{
    PoolInfo*poolinfo=(PoolInfo*)arg;
    int addrlen=sizeof(struct sockaddr_in);
    //5.阻塞并等待客户端连接
    while(1)
    {
        struct SockInfo* pinfo;
        pinfo=(struct SockInfo*)malloc(sizeof(struct SockInfo));

        pinfo->fd=accept(poolinfo->fd,(struct sockaddr*)&pinfo->addr,&addrlen);

        if(pinfo->fd==-1)
        {
            perror("accept creat failed....\n");
            break;
        } 
        //6.添加通讯的任务
        threadPoolAdd(poolinfo->pool,working,pinfo);
    }

    close(poolinfo->fd);
    return;
}
void  working(void*arg)
{  
    struct SockInfo *pinfo=(struct ScokInfo*)arg;
    //连接成功，打印客户端的IP和端口信息
    char ip[32];
    printf("连接的客户端IP: %s ,端口: %d\n",
            inet_ntop(AF_INET,&pinfo->addr.sin_addr.s_addr,ip,sizeof(ip)),
            ntohs(pinfo->addr.sin_port));
    
    //7.通讯
    while (1)
    {
        //接受数据
        char buff[1024];
        int len=recv(pinfo->fd,buff,sizeof(buff),0);
        if(len>0)
        {
            printf("client say:%s\n",buff);
            send(pinfo->fd,buff,len,0);
        }else if(len==0)
        {
            printf("客户端断开了链接...\n");
            break;
        }else
        {
            perror("recv\n");
            break;
        }
    }

    //关闭文件描述符
     close(pinfo->fd);
    return;  
}
