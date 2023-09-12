#ifndef __MYSQL__H__
#define __MYSQL__H__

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include<string>


class MysqlPool//连接池类
{
private:
MysqlPool();
~MysqlPool();
MysqlPool(const MysqlPool& mp) = delete;//禁用拷贝构造
MysqlPool& operator=(const MysqlPool& mp) = delete;//禁用操作符“=”
    
public:
sql::Connection* GetConnection();//进行连接
void ReleaseConnection(sql::Connection* conn);//释放连接
static MysqlPool *GetInstance();//创建连接类
void init(std::string host,std::string user,std::string password,std::string databasename,int maxconn);
std::string m_host;			 //数据库地址
std::string m_User;		 //登陆数据库用户名
std::string m_PassWord;	 //登陆数据库密码
std::string m_DatabaseName; //使用数据库名
std::queue<sql::Connection*> connections;//连接队列
std::mutex mutex;//互斥锁
std::condition_variable cond;//通知
    
};


#endif