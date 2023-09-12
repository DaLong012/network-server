#include"http.h"

char buf[2048];
void readtask(int fd)//客户端任务
{
    ssize_t  bytesReceived=recv(fd,buf,sizeof(buf),0);
    if(bytesReceived>0)
    {
        buf[bytesReceived] = '\0';
        std::string httpRequest=buf;
        JXHTTPREQUEST(httpRequest,fd);   
    }
    else if(bytesReceived==0)
    {
        std::cout << "Connection closed by peer." << std::endl;
    }
    else
    {
        perror("recv failed");

    }
    close(fd);
}

std::pair<std::string, std::string> parseRequestLine(const std::string& requestLine)//解析请求行
 {
    std::istringstream iss(requestLine);
    std::string method, path, protocol;

    if (!(iss >> method >> path >> protocol)) {
        throw std::runtime_error("Failed to parse request line.");
    }

    return {method, path};
}
void JXHTTPREQUEST(std::string httpRequest,int fd)//解析请求并响应
{
    size_t emptyLinePos = httpRequest.find("\r\n\r\n");
    if (emptyLinePos != std::string::npos) 
    {
        std::string requestLineAndHeaders = httpRequest.substr(0, emptyLinePos);
        std::string requestBody = httpRequest.substr(emptyLinePos + 4);  // Skip the "\r\n\r\n" characters
     auto [method, path] = parseRequestLine(requestLineAndHeaders);
     if(method.compare("GET") == 0)
     {
        MyResponse(path,fd);
     }
     else if(method.compare("POST") == 0)
     {
        UserResponse(path,fd,requestBody);

     }
    }

}
std::string readFile(const std::string& filename) //获取文件
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "File not found";
    }

    std::ostringstream content;
    content << file.rdbuf();
    file.close();

    return content.str();
}

std::string determineContentType(const std::string& filename) //判断文件类型
{
    static std::map<std::string, std::string> extensionToContentType = {
        {"html", "text/html"},
        {"css", "text/css"},
        {"js", "application/javascript"},
        {"jpg", "image/jpeg"},
        {"png", "image/png"}
        // Add more mappings as needed
    };

    size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos + 1);
        auto it = extensionToContentType.find(extension);
        if (it != extensionToContentType.end()) {
            return it->second;
        }
    }

    return "application/octet-stream";  // Default to binary data
}

void MyResponse(std::string path,int fd)//文件请求响应
{
    std::string response;
    std::string projectRoot = "/home/dalong/Project1";
    if(path=="/")
    {
        path="/index.html";
    }
    std::string filename = projectRoot + path;
    std::string contentType = determineContentType(filename);
    std::cout << "文件路径：" << filename << std::endl;
    // 打开文件
    std::ifstream file(filename);
    if (!file.is_open()) {
        // 文件打开错误，构建错误响应
        response = "HTTP/1.1 404 Not Found\r\n";
        response += "Content-Type: text/plain; charset=utf-8\r\n";
        response += "\r\n";
        response += "文件未找到或无法打开";
    } else {
        // 文件成功打开，读取文件内容并构建正常响应
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + contentType + "\r\n";
        response += "\r\n";
        response += readFile(filename); // 读取并添加文件内容
        file.close();
    }

    send(fd, response.c_str(), response.size(), 0);
}

std::string generateResponsel(const std::string status, const std::string message)//响应内容
 {
    std::string response = "HTTP/1.1 " + status + "\r\n";
    response += "Content-Type: application/json\r\n"; // 使用JSON作为Content-Type
    response += "\r\n";
    response += "{\n";
    response += "    \"success\": " +std::string (status == "200 OK" ? "true" : "false")+ ",\n"; // 根据状态码设置 success
    response += "    \"message\": \"" + message + "\"\n";
    response += "}\n";
    return response;
}

void  UserResponse(std::string path,int fd,std::string requestBody)//注册和登入响应
{
    std::string response;
    if (path=="/register") 
    {
        if(Isregister(requestBody)==false)
        {
            std::string response = generateResponsel("401 Unauthorized", "Registration Failed");
            send(fd, response.c_str(), response.size(), 0);
        }
        else
        {
            std::string response = generateResponsel("200 OK", "Registration successful");
            send(fd, response.c_str(), response.size(), 0);
        }
        
        
    }
    else if (path=="/login") {
        if(Islogin(requestBody)==false)
        {
            std::string response = generateResponsel("401 Unauthorized", "Login Failed");
            send(fd, response.c_str(), response.size(), 0);
        }
        else
        {
            std::string response = generateResponsel("200 OK", "Login successful");
            send(fd, response.c_str(), response.size(), 0);
        }
        
    }
}
bool Islogin(std::string requestBody)//判断用户是否存在
{
    sql::ResultSet* resultSet;
    sql::PreparedStatement* preparedStatement;
    sql::Connection* conn=MysqlPool::GetInstance()->GetConnection();
    std::string username1 = extractValue(requestBody, "username");
    std::string password1 = extractValue(requestBody, "password");
    preparedStatement = conn->prepareStatement("SELECT * FROM user WHERE username = ?");
    preparedStatement->setString(1, username1);
    resultSet = preparedStatement->executeQuery();
    if (resultSet->next())
    {
        std::string username2 = resultSet->getString("username");
        std::string password2 = resultSet->getString("passwd");
        delete resultSet;
        delete preparedStatement;
        MysqlPool::GetInstance()->ReleaseConnection(conn);
        if(username1==username2&&password1==password2)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        delete resultSet;
        delete preparedStatement;
        MysqlPool::GetInstance()->ReleaseConnection(conn);
        return false;
    }
}
bool Isregister(std::string requestBody)//判断用户是否存在，如果存在则返回，不存在则注册
{
    sql::ResultSet* resultSet;
    sql::PreparedStatement* preparedStatement;
    sql::Connection* conn=MysqlPool::GetInstance()->GetConnection();
    std::string username1 = extractValue(requestBody, "username");
    std::string password1 = extractValue(requestBody, "password");
    preparedStatement = conn->prepareStatement("SELECT * FROM user WHERE username = ?");
    preparedStatement->setString(1, username1);
    resultSet = preparedStatement->executeQuery();
    if (resultSet->next())
    { 
       delete resultSet;
       delete preparedStatement;
       MysqlPool::GetInstance()->ReleaseConnection(conn);
       return false;
    }
    else
    {
        preparedStatement = conn->prepareStatement("INSERT INTO user (username, passwd) VALUES (?, ?)");
        preparedStatement->setString(1, username1);
        preparedStatement->setString(2, password1);
        preparedStatement->executeUpdate(); 
    }
    delete resultSet;
    delete preparedStatement;
    MysqlPool::GetInstance()->ReleaseConnection(conn);
    return true;
}

std::string extractValue(const std::string& json, const std::string& key) //从请求体中解析用户和密码
{
    size_t pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) {
        return "";
    }
    
    size_t colonPos = json.find(":", pos);
    size_t valueStartPos = json.find("\"", colonPos);
    size_t valueEndPos = json.find("\"", valueStartPos + 1);

    if (colonPos == std::string::npos || valueStartPos == std::string::npos || valueEndPos == std::string::npos) {
        return "";
    }

    return json.substr(valueStartPos + 1, valueEndPos - valueStartPos - 1);
}