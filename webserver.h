#ifndef __WEBSERVER__H__
#define __WEBSERVER__H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include<fcntl.h>
#include <sys/epoll.h>
#include"threadpool.h"
#include"http.h"

#define PORT 9999 //端口宏定义

class webserver//服务器类
{
private:
    int   MAX_EVENT=128;//设置最大连接数
public:
    void server();//服务器具体运行
    void dealread(int fd);//监听到读事件的处理方式
    void dealwrite(int fd);//监听到写事件的处理方式
};


#endif