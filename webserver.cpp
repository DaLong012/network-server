#include"webserver.h"

void webserver::server()//用于与客户端进行连接，并监听事件，将任务添加到任务队列
{
    int lfd=socket(AF_INET,SOCK_STREAM,0);//创建监听套接字

    struct sockaddr_in ad;
    bzero(&ad,sizeof(ad));//初始化地址
    ad.sin_family=AF_INET;//接口设置
    ad.sin_port=htons(PORT);//端口设置
    ad.sin_addr.s_addr=htonl(INADDR_ANY);//IP设置

    int flag = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));//进行端口复用

    bind(lfd,(struct sockaddr*)&ad,sizeof(ad));//绑定地址
    listen(lfd,-1);//监听客户端同时连接数

   int efd=epoll_create(12);//创建红黑树事件表
   epoll_event evt,events[MAX_EVENT];
   evt.events=EPOLLIN;//设置为监听读事件并默认为LT触发模式
   evt.data.fd=lfd;
   epoll_ctl(efd,EPOLL_CTL_ADD,lfd,&evt);
   while(1)
   {
    int number=epoll_wait(efd,events,MAX_EVENT,-1);//返回监听到的事件
    for(int i=0;i<number;i++)
    {
        int sfd=events[i].data.fd;
        if(sfd==lfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int cfd=accept(lfd,(struct sockaddr*)&client_address,&client_addrlength);//连接到的客户端
            int flags=fcntl(cfd,F_GETFL);//获取状态
            flags|=O_NONBLOCK;//设为非阻塞状态
            fcntl(cfd,F_SETFL,flags);//改为非阻塞状态
            epoll_event evts;
            evts.events=EPOLLIN | EPOLLET;//设置为监听读事件并默认为ET触发模式
            evts.data.fd=cfd;
            epoll_ctl(efd,EPOLL_CTL_ADD,cfd,&evts);
        }
        else if(events[i].events&EPOLLIN)
        {
            dealread(sfd);//调用处理读事件的函数
        }
        else if(events[i].events&EPOLLOUT)
        {
            dealwrite(sfd);//调用处理写事件的函数
        }
    }
   }
   close(lfd);
   close(efd);
}

void webserver::dealread(int fd)
{
    ThreadPool::GetInstance().addqueue(readtask,fd);//将任务添加到队列
}
void webserver::dealwrite(int fd)
{

}
void webserver::Log()
{
    logger::GetInstance()->open("./test.log");
    logger::GetInstance()->setmax(1024);
}
