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
#include <filesystem>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/epoll.h>

using namespace std;

constexpr char IP[]="127.0.0.1";
const int PORT=8081;
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
    void establishing_session(int cliend_fd); 
    int establishing_data_connection(int cliend_fd);
    void server_read_catelog(int data_fd,vector<string> result);
    void server_download_file(int data_fd,vector<string> result);
    void server_upload_file(int data_fd,vector<string> result);
    vector<string> split(string order);
    void printf_permission(filesystem::directory_entry path);
};