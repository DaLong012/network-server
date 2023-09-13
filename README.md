# network-server
学习C++服务器项目
Linux下用C++实现Web服务器（手写实现简易的网页注册，登录服务器）
使用 线程池 + 非阻塞socket + epoll(ET) + 事件处理(Reactor模式) 的并发模型
手写http.cpp实现请求报文，支持解析GET和POST请求，并响应
访问服务器数据库实现web端用户注册、登录功能，可以请求服务器图片和视频文件
