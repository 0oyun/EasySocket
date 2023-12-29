
#include "client.hpp"
using namespace std;

const int RECV_BUF_SIZE = 1024;

void Client::thread_function(int *client_socket)
{
    char recv_buf[RECV_BUF_SIZE];
    int *socket = (int *)client_socket;
    string src_id = "";
    string msg = "";

    while (1)
    {
        if (recv(*socket, recv_buf, sizeof(recv_buf), 0) != 0)
        {

            switch (recv_buf[0])
            {
            case 'i':
                cout << "[system] Your Client ID: (" << string(recv_buf + 2) << ")" << endl;
                cout << "[system] >> " << flush;
                break;
            case 't': // time
                cout << "\n[server] ";
                cout << "The current time is " << string(recv_buf + 2) << endl;
                cout << "[system] >> " << flush;
                break;
            case 'n': // name
                cout << "\n[server] ";
                cout << "The machine name is " << recv_buf + 2 << endl;
                cout << "[system] >> " << flush;
                break;
            case 'l': // list
                cout << "\n[server] ";
                cout << recv_buf + 2 << endl;
                cout << "[system] >> " << flush;
                break;
            case 'r': // recv msg
            {
                int i = 2;
                src_id = "";
                msg = "";
                while (recv_buf[i] != '\x1D')
                {
                    src_id += recv_buf[i];
                    i++;
                }
                i++;
                while (recv_buf[i] != '\0')
                {
                    msg += recv_buf[i];
                    i++;
                }
                cout << "You have a new message from [" << src_id << "] : " << msg << endl;
                cout << "[system] >> " << flush;
                break;
            }

            case 's': // send msg back
            {
                switch (recv_buf[2])
                {
                case '0':
                    cout << "Send failed." << endl;
                    break;
                case '1':
                    cout << "Send succeeded." << endl;
                    break;
                case '2':
                    cout << "Invalid ID." << endl;
                    break;
                default:
                    break;
                }
                cout << "[system] >> " << flush;
                break;
            }
            default:
                break;
            }
            memset(recv_buf, 0, sizeof(recv_buf));
        }
    }
}

void Client::checkID()
{
    if (connect_status)
    {
        if (send(client_socket, "i", 2, 0) == -1)
        {
            cout << "[system] Send failed.\n";
        }
    }
    else
    {
        cout << "[system] No connection.\n";
    }
    return;
}

bool Client::connect()
{
    if (connect_status)
    {
        cout << "[system] Already connected.\n";
        return true;
    }

    char ip[16] = "0.0.0.0";
    int port;

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == -1)
    {
        cout << "[system] Socket error.\n";
        exit(-1);
    }

    // cout << "[system] Input the IP >> ";
    // cin >> ip;

    // cout << "[system] Input the port number >> ";
    // cin >> port;
    port = 1763;

    sock_addr_in.sin_family = AF_INET;
    sock_addr_in.sin_port = htons(port);
    sock_addr_in.sin_addr.s_addr = inet_addr(ip);

    if (::connect(client_socket, (struct sockaddr *)&sock_addr_in, sizeof(sock_addr_in)) == 0)
    {

        std::thread t(&Client::thread_function, this, &client_socket);

        send(client_socket, "c", 2, 0);
        connect_status = true;
        cout << "[system] Connection succeeded.\n";
        if (t.joinable())
        {
            t.detach();
        }
        return true;
    }
    else
    {
        cout << "[system] Connection failed.\n";
        return false;
    }
}

bool Client::disconnect()
{
    if (connect_status)
    {
        // send(client_socket, "d", 2, 0);
        // send(client_socket, "d", 2, 0) == -1
        if (shutdown(client_socket, SHUT_RDWR) == -1) // TODO :???
        {
            cout << "Disconnection request failed.\n";
            return false;
        }
        else
        {
            cout << "Disconnection request succeeded.\n";
            connect_status = false;
            return true;
        }
    }
    else
    {
        cout << "[system] No connection.\n";
        return false;
    }
}

void Client::request_time()
{
    if (connect_status)
    {
        if (send(client_socket, "t", 2, 0) == -1)
        {
            cout << "[system] Send failed.\n";
        }
    }
    else
    {
        cout << "[system] No connection.\n";
    }
    return;
}

void Client::request_name()
{
    if (connect_status)
    {
        if (send(client_socket, "n", 2, 0) == -1)
        {
            cout << "[system] Send failed.\n";
        }
    }
    else
    {
        cout << "[system] No connection.\n";
    }
    return;
}

void Client::request_client_list()
{
    if (connect_status)
    {
        if (send(client_socket, "l", 2, 0) == -1)
        {
            cout << "[system] Send failed.\n";
        }
    }
    else
    {
        cout << "[system] No connection.\n";
    }
    return;
}

void Client::send_msg()
{
    if (connect_status)
    {
        string recv_id;
        string msg;
        string send_buf;

        cout << "[system] Input receiver's id >> ";
        cin >> recv_id;

        cout << "[system] Input your message >> ";
        getline(cin, msg);
        getline(cin, msg);
        send_buf = "s\x1D" + recv_id + "\x1D" + msg;

        if (send(client_socket, send_buf.c_str(), send_buf.length(), 0) == -1)
        {
            cout << "[system] Send failed.\n";
        }
        else
        {
            cout << "[system] Successfully sent.\n";
        }
    }
    else
    {
        cout << "[system] No connection.\n";
    }
    return;
}

void Client::quit()
{
    if (connect_status)
        disconnect();
    cout << "bye." << endl;
    exit(0);
}

void Client::menu()
{
    char choice;
    cout << "\n[system] Please choose the function you want to use:\n"
         << "[system] 0. Check current ID\n"
         << "[system] 1. Connect\n"
         << "[system] 2. Request current time\n"
         << "[system] 3. Request machine name\n"
         << "[system] 4. Request client list\n"
         << "[system] 5. Send message\n"
         << "[system] 6. Disconnect\n"
         << "[system] 7. Quit\n"
         << "[system] >> ";
    string msg;
    while (1)
    {
        cin >> choice;
        getline(cin, msg);
        switch (choice)
        {
        case '0':
            checkID();
            break;
        case '1':
            connect();
            cout << "[system] >> " << flush;
            break;
        case '2':
            request_time();
            break;
        case '3':
            request_name();
            break;
        case '4':
            request_client_list();
            break;
        case '5':
            send_msg();
            break;
        case '6':
            disconnect();
            cout << "[system] >> " << flush;
            break;
        case '7':
            quit();
            break;
        default:
            cout << "[system] Invalid input.\n";
            break;
        }
    }
}