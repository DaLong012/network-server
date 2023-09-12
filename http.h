#ifndef __HTTP__H__
#define __HTTP__H__

#include"Mysql.h"
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <strings.h>
#include <unistd.h>
#include <fstream>

void readtask(int fd);//读事件任务
std::pair<std::string, std::string> parseRequestLine(const std::string& requestLine);//解析请求行
void JXHTTPREQUEST(std::string httpRequest,int fd);//解析浏览器请求
std::string readFile(const std::string& filename);//获取请求的文件和取出其内容
std::string determineContentType(const std::string& filename);//解析文件类型
void MyResponse(std::string path,int fd);//get响应
std::string generateResponse(const std::string& status, const std::string& message);//响应内容
void  UserResponse(std::string path,int fd,std::string requestBody);//post响应
bool Isregister(std::string requestBody);//判断是否注册成功
bool Islogin(std::string requestBody);//判断是否登入成功
std::string extractValue(const std::string& json, const std::string& key);//解析post请求体的用户和密码

#endif