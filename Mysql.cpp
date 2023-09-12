#include"Mysql.h"

MysqlPool::MysqlPool()
{
    init("tcp://127.0.0.1:3306","root","173214","yourdb",5);//创建连接池
}
MysqlPool::~MysqlPool()//销毁连接池
{
    for (int i = 0; i < connections.size(); ++i)
     {
        sql::Connection* conn = connections.front();
        connections.pop();
        delete conn;
     }
}

 MysqlPool *MysqlPool::GetInstance()//单列模式
{
    static MysqlPool mypool;
    return &mypool;
}

void MysqlPool::init(std::string host,std::string user,std::string password,std::string databasename,int maxconn)//初始化数据库并创建连接池
{
    m_host=host;
    m_User=user;
    m_PassWord=password;
    m_DatabaseName=databasename;
    sql::mysql::MySQL_Driver *driver;
    driver = sql::mysql::get_mysql_driver_instance();
    for(int i=0;i<maxconn;i++)
    {
        sql::Connection *con;
        con = driver->connect(host, user,password);
        if(con!=nullptr)
        {
            con->setSchema(databasename);
            connections.push(con);
        }
         else
          {
                std::cerr << "Failed to connect to MySQL database." << std::endl;
          }

    }

}

sql::Connection* MysqlPool::GetConnection() //从连接池取出一个空闲连接给客户端
{
        std::unique_lock<std::mutex> lock(mutex);
        if (connections.empty()) {
            cond.wait(lock, [this]() { return !connections.empty(); });
        }
        sql::Connection* conn = connections.front();
        connections.pop();
        return conn;
}

void MysqlPool::ReleaseConnection(sql::Connection* conn) //释放连接，并返回到连接池
{
        std::lock_guard<std::mutex> lock(mutex);
        connections.push(conn);
        cond.notify_one();
}