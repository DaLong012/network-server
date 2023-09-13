# network-server（学习C++服务器项目）
Linux下用C++实现Web服务器（手写实现简易的网页注册，登录服务器）：                                                                                                             
使用 线程池 + 非阻塞socket + epoll(ET) + 事件处理(Reactor模式) 的并发模型                                                                                                                                               
手写http.cpp实现请求报文，支持解析GET和POST请求，并响应                                          
用MySql数据库创建一个数据库，再使用创建的数据库创建一个用户表实现数据记录                                               
手写Mysql.cpp实现用户进行注册和登录交互的连接池            
手写threadpool.h实现线程池                                                                
手写webserver.cpp实现服务器和其运行                      
# Demo演示（网页随便写的，功能实现就行）
注册功能：

https://github.com/DaLong012/network-server/assets/129527031/85db2168-f43f-4a99-9e55-3988a8c2c60b

登录功能：

https://github.com/DaLong012/network-server/assets/129527031/720fe78e-7774-4910-b4be-367a42c82e86

