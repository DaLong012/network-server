#pragma once
#include<string>
#include<string.h>
#include<fstream>
#include<stdarg.h>
#include<mutex>

#define debug(format,...) \
        logger::GetInstance()->log(logger::DEBUG,__FILE__,__LINE__,##__VA_ARGS__)
#define info(format,...) \
        logger::GetInstance()->log(logger::INFO,__FILE__,__LINE__,##__VA_ARGS__)
#define warn(format,...) \
        logger::GetInstance()->log(logger::WARN,__FILE__,__LINE__,##__VA_ARGS__)
#define error(format,...) \
        logger::GetInstance()->log(logger::ERROR,__FILE__,__LINE__,##__VA_ARGS__)
#define fatal(format,...) \
        logger::GetInstance()->log(logger::FATAL,__FILE__,__LINE__,##__VA_ARGS__)

class logger
{
public:
    enum Level
    {
        DEBUG=0,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT

    };
    void log(Level level,const char *file,int line,const char * format,...);
    static logger *GetInstance(); 
    void open(const std::string filename);
    void close();
    void level(Level level);
    void setmax(int bytes);
    void rotate();
private:
     logger();
    ~logger();
    logger(const logger& lg)=delete;
    logger& operator=(const logger& lg)=delete;
    std::string m_filename;
    std::ofstream m_fout;
    Level m_level;
    std::mutex mutx;
    int m_max;
    int m_len;
    static const char* s_level[LEVEL_COUNT];
};
