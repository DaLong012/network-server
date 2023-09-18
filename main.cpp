#include<iostream>
#include"webserver.h"
int main()
{
    webserver web;//创建服务器
    web.Log();//打开日志系统
    web.server();//服务器运行
    
}