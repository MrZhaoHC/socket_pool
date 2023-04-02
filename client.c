#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
//socket头文件
#include<arpa/inet.h>

int main()
{
    //1.创建通讯的套接字
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1)
    {
        perror("socket creat failed.....\n");
        return -1;
    }
    //2.链接服务器
    struct sockaddr_in saddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(9999);
    inet_pton(AF_INET,"192.168.88.137",&saddr.sin_addr.s_addr);
    int ret=connect(fd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret==-1)
    {
        perror("bind creat failed....\n");
        return -1;
    }

    int number=0;
    //3.通讯
    while (1)
    {
        //发送数据
        char buff[1024];
        sprintf(buff,"你好，我是socket %d...\n",number++);
        send(fd,buff,strlen(buff)+1,0);

        //接受数据
        memset(buff,0,sizeof(buff));
        int len=recv(fd,buff,sizeof(buff),0);
        if(len>0)
        {
            printf("server say:%s\n",buff);
        }else if(len==0)
        {
            printf("服务器断开了链接...\n");
            break;
        }else
        {
            perror("recv\n");
            break;
        }
        sleep(1);
    }

    //关闭文件描述符
     close(fd);

    return 0;    
}
