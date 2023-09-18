#include"logger.h"
#include<time.h>

const char* logger::s_level[LEVEL_COUNT]={
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

logger::logger()
:m_level(DEBUG),m_max(0),m_len(0)
{

}
logger::~logger()
{
    close();
}

logger *logger::GetInstance()
{
    static logger lg;
    return &lg;
}
void logger::open(const std::string filename)
{
   m_filename=filename;  
   m_fout.open(filename,std::ios::app);
   if(m_fout.fail())
   {
        throw std::logic_error("open file failed"+filename);
   }
   m_fout.seekp(0,std::ios::end);
   m_len=m_fout.tellp();
}

void logger::close()
{
    m_fout.close();
}

void logger::level(Level level)
{
    m_level=level;
}
void logger::setmax(int bytes)
{
    m_max=bytes;
}
void logger::log(Level level,const char *file,int line,const char * format,...)
{
    if(m_level>level)return;
    if(m_fout.fail())
   {
        throw std::logic_error("open file failed"+m_filename);
   }
   std::unique_lock<std::mutex> logg(mutx);
   time_t ticks=time(NULL);
   struct tm * ptn=localtime(&ticks);
   char timestamp[32];
   memset(timestamp,0,sizeof(timestamp));
   strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",ptn);

   const char * fmt="%s %s %S:%d ";
   int size=snprintf(NULL,0,fmt,timestamp,s_level[level],file,line);
   if(size>0)
   {
        char *buffer=new char[size+1];
        snprintf(buffer,size+1,fmt,timestamp,s_level[level],file,line);
        buffer[size]='\0';
        m_fout<<buffer;
        m_len+=size;
        delete buffer;
   }
   va_list arg_ptr;
   va_start(arg_ptr,format);
   size=vsnprintf(NULL,0,format,arg_ptr);
   va_end(arg_ptr);
   if(size>0)
   {
        char * cnt=new char[size+1];
        va_start(arg_ptr,format);
        vsnprintf(cnt,size+1,format,arg_ptr);
        va_end(arg_ptr);
        cnt[size]='\0';
        m_fout<<cnt;
         m_len+=size;
        delete cnt;
   }
   m_fout<<"\n";
   m_fout.flush();
   if(m_len>=m_max && m_max>0)
   {
        rotate();
   }
}

void logger::rotate()
{
    close();
    time_t ticks=time(NULL);
    struct tm * ptn=localtime(&ticks);
    char timestamp[32];
    memset(timestamp,0,sizeof(timestamp));
    strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",ptn);
    std::string file_name=m_filename+timestamp;
    if(rename(m_filename.c_str(),file_name.c_str())!=0)
    {
        throw std::logic_error("rename log  file failed:"+std::string(strerror(errno)));
    }
    open(m_filename);
}