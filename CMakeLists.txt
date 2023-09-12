cmake_minimum_required(VERSION 3.02)
project(xm1)
add_executable(server main.cpp http.cpp Mysql.cpp webserver.cpp)
target_link_libraries(server mysqlcppconn)
