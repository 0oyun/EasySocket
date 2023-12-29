// client.hpp

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

class Client
{
private:
    int client_socket;
    bool connect_status = false;
    sockaddr_in sock_addr_in;

    // 线程函数
    void thread_function(int *client_socket);

    void checkID();
    // 连接
    bool connect();

    // 断开连接
    bool disconnect();

    // 请求当前时间
    void request_time();

    // 请求机器名称
    void request_name();

    // 请求活动连接列表
    void request_client_list();

    // 发消息
    void send_msg();

    // 退出
    void quit();

public:

    void menu();
};

#endif