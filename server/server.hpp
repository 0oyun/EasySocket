// server.hpp

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <arpa/inet.h>
#include <ctime>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <vector>

class Server
{
private:
    std::vector<std::pair<int *, int> > clients; // <client_socket,id>
    int listen_socket;
    int port = 1763;
    sockaddr_in listen_sock_addr_in;
    
    void showID(int *client_socket);
    void show_time(int *client_socket);
    void show_name(int *client_socket);
    void show_client_list(int *client_socket);
    void forward(int *client_socket, char *recv_buf);

    void thread_function(int *client_socket);

public:
    Server();
    ~Server();
    
    void start();
};

#endif