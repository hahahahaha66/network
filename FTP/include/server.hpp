#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <random>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/epoll.h>

using namespace std;

constexpr char IP[]="127.0.0.1";
const int PORT=10000;
const int MAX_EVENT=10;


class server{
    public:
    int socket_fd;
    struct sockaddr_in sock_addr;
    int epfd;
    struct epoll_event ev, event[MAX_EVENT];
    server();
    ~server();
    void server_accept_with_comminicate();
    void set_unlocking(int fd);
    void establishing_session(int client_fd); 
    void establishing_data_connection(int client_fd);
};