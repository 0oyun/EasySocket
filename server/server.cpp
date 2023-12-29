#include "server.hpp"
using namespace std;

void Server::thread_function(int *client_socket)
{
    char recv_buf[1024];
    while (1)
    {
        if (recv(*client_socket, recv_buf, sizeof(recv_buf), 0) > 0)
        {
            switch (recv_buf[0])
            {
            case 'i':
                showID(client_socket);
                break;
            case 'c':
                cout << "[system] Client(" << *client_socket << ") has been connected part1" << endl;
                break;
            case 't':
                show_time(client_socket);
                break;
            case 'n':
                show_name(client_socket);
                break;
            case 'l':
                show_client_list(client_socket);
                break;
            case 's':
                forward(client_socket, recv_buf);
                break;
            default:
                cout << "[system] Invalid command." << endl;
            }
            memset(recv_buf, 0, sizeof(recv_buf));
        }
        else
        {
            cout << "shut down received" << endl;
            for (auto iter = clients.begin(); iter != clients.end();)
            {
                if (*(*iter).first == *client_socket) //
                {
                    iter = clients.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
            cout << "[system] Client(" << *client_socket << ") has been disconnected" << endl;
            break;
        }
    }
}

Server::Server()
{
    clients.clear();
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    listen_sock_addr_in.sin_family = AF_INET;
    listen_sock_addr_in.sin_addr.s_addr = INADDR_ANY;
    listen_sock_addr_in.sin_port = htons(port);
    cout << "[initialize] Socket initialized!" << endl;
}
Server::~Server()
{
    shutdown(listen_socket, SHUT_RDWR);
    close(listen_socket);
    cout << "[system] Socket closed!" << endl;
}
void Server::start()
{
    auto bindfd = ::bind(listen_socket, (struct sockaddr *)&listen_sock_addr_in, sizeof(listen_sock_addr_in));
    if (bindfd < 0)
    {
        cout << "[system] Bind failed!" << endl;
        exit(-1);
    }
    if (listen(listen_socket, 100) < 0)
    {
        cout << "[system] Listen failed!" << endl;
        exit(-1);
    }
    while (1)
    {
        int *client_socket = new int;

        *client_socket = accept(listen_socket, 0, 0);

        clients.push_back(make_pair(client_socket, int(*client_socket)));
        std::thread t(&Server::thread_function, this, client_socket);
        // if (t.joinable())
        // {
        t.detach();
        // }
    }
}

void Server::showID(int *client_socket)
{
    string send_buf = "i\x1D";
    send_buf += to_string(*client_socket);
    if (send(*client_socket, send_buf.c_str(), send_buf.length(), 0) == -1)
    {
        cout << "[system] Send failure " << endl;
    }
}

void Server::show_time(int *client_socket)
{
    time_t current_time = time(NULL);
    char *c_time = ctime(&current_time);
    cout << "[system] Current time is : " << c_time << endl;
    string send_buf = "t\x1D" + string(c_time);
    if (send(*client_socket, send_buf.c_str(), send_buf.length(), 0) == -1)
    {
        cout << "[system] Send failed " << endl;
    }
}
void Server::show_name(int *client_socket)
{
    string send_buf = "n\x1D";

    const int HOST_NAME_MAX = 256;
    std::vector<char> hostname(HOST_NAME_MAX);

    if (gethostname(hostname.data(), HOST_NAME_MAX) == 0)
    {
        std::cout << "[system] Host name: " << hostname.data() << std::endl;
    }
    else
    {
        std::cerr << "[system] Error getting host name" << std::endl;
    }
    send_buf += hostname.data();
    if (send(*client_socket, send_buf.c_str(), send_buf.length(), 0) == -1)
    {
        cout << "[system] Send failure " << endl;
    }
}
void Server::show_client_list(int *client_socket)
{
    string send_buf = "l\x1D";
    send_buf += "client list:\n";
    // send_buf+="-------------------------------\n";
    for (auto it : clients)
    {
        send_buf += to_string(it.second) + "\n";
    }
    // send_buf+="-------------------------------\n";

    cout << send_buf << endl;

    if (send(*client_socket, send_buf.c_str(), send_buf.length(), 0) == -1)
    {
        cout << "[system] Send failure " << endl;
    }
}
void Server::forward(int *client_socket, char *recv_buf)
{
    string dst_id = "";
    string msg = "";
    string send_buf = "r\x1D";
    string back_buf = "s\x1D";

    bool flag = false;
    int i = 2;
    while (recv_buf[i] != '\x1D')
    {
        dst_id += recv_buf[i];
        i++;
    }
    i++;
    while (recv_buf[i] != '\0')
    {
        msg += recv_buf[i];
        i++;
    }
    send_buf += to_string(*client_socket) + "\x1D" + msg;
    for (auto it : clients)
    {
        if (to_string(it.second) == dst_id)
        {
            if (send(*(it.first), send_buf.c_str(), send_buf.length(), 0) == -1)
            {
                back_buf += "0";
                cout << "[system] Send failure " << endl;
            }
            else
            {
                back_buf += "1";
                cout << "[system] Send success" << endl;
                cout << "[system] Message from " << to_string(*client_socket) << " to " << dst_id << ": " << endl;
                cout << msg << endl;
            }
            flag = true;
        }
    }
    if (!flag)
    {
        back_buf += "2";
        cout << "[system] Invalid ID." << endl;
    }
}