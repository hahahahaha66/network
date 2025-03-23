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

const char* IP="0.0.0.0";
const int PORT=2100;
const int MAX_EVENT=10;
 
void set_unlocking(int fd) {
    int flags = fcntl(fd,F_GETFL,0);
    if(fcntl(fd,F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl failed");
    }
}

void transferring_file(int cliend_fd) {
    int file_fd=socket(AF_INET,SOCK_STREAM,0);
    if(file_fd == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(1024,5000);

    char pasv_resp[64];
    struct sockaddr_in file_addr;
    int file_port=0;
    do {int file_port = dis(gen);
        file_addr.sin_port=htons(file_port);
        inet_pton(AF_INET,IP,&file_addr.sin_addr);
        file_addr.sin_family=AF_INET;
    } while(bind(file_fd,(sockaddr*)&file_addr,sizeof(file_addr) == -1));
    snprintf(pasv_resp, sizeof(pasv_resp), "227 entering passive mode (192,168,1,1,%d,%d)\r\n",file_port/256,file_port%256);
    send(cliend_fd,pasv_resp,strlen(pasv_resp),0);
    

    if(listen(file_fd,10) == -1) {
        perror("listen failed");
        close(file_fd);
        exit(EXIT_FAILURE);
    }

    int cliend_file = accept(file_fd,NULL,NULL);
    if(cliend_file == -1) {
        perror("accept failed");
        close(file_fd);
        exit(EXIT_FAILURE);
    }

    set_unlocking(cliend_file);

    while(1) {
        char buffer[4096];
        memset(buffer,0,sizeof(buffer));
        int topic_num = recv(cliend_file,buffer,sizeof(buffer),0);
        ofstream file(buffer,ios::binary | ios::app);
        memset(buffer,0,sizeof(buffer));
        int byte_received = 0;
        while((byte_received=recv(cliend_file, buffer, sizeof(buffer), 0)) > 0){
            file.write(buffer,byte_received);
            memset(buffer,0,sizeof(buffer));
        }
        if(byte_received == -1){
            perror("recv failed");
            close(cliend_fd);
        }
        cout<<"file received successfully"<<endl;
    }
    close(file_fd);
}

void establish_session (int cliend_fd) {
    while(1){
        char buffer[1024];
        int bytes = recv(cliend_fd,buffer,sizeof(buffer),0);
        if(bytes>0){
            send(cliend_fd,buffer,bytes,0);
            buffer[bytes]='\0';
            if(strcmp(buffer,"PASV")==0) {
                thread th(transferring_file,cliend_fd);
                th.detach();
            }
            else if(strcmp(buffer,"STOR")==0) {
                thread th(transferring_file,cliend_fd);
                th.detach();
            }
            else if(strcmp(buffer,"RETR")==0) {
                thread th(transferring_file,cliend_fd);
                th.detach();
            }
            else {
                cout<<buffer<<endl;
            }
        }
        else {
            close(cliend_fd);
        }
    }
    
}

int main() {
    int socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
        return 1;
    }
    struct sockaddr_in sock_addr;
    sock_addr.sin_family=AF_INET;
    sock_addr.sin_port=htons(PORT);
    inet_pton(AF_INET,IP,&sock_addr.sin_addr);
    int bind_return = bind(socket_fd,(sockaddr* )&sock_addr,sizeof(sock_addr));
    if(bind_return == -1){
        perror("bind failed");
        exit(EXIT_FAILURE);
        return 1;
    }

    int epfd = epoll_create(1);
    struct epoll_event ev, event[MAX_EVENT];

    listen(socket_fd,10);

    set_unlocking(socket_fd);
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epfd,EPOLL_CTL_ADD,socket_fd,&ev) ==-1) {
        perror("epoll_ctl failed");
    }

    while(1){
        int nfds = epoll_wait(epfd,event,MAX_EVENT,-1);
        for(int i = 0;i < nfds; i++){
            if(event[i].data.fd == socket_fd) {
                int client_fd = accept(socket_fd,NULL,NULL);
                if(client_fd == -1) {
                    perror("accept failed");
                    close(socket_fd);
                    continue;
                }
                set_unlocking(client_fd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&ev) == -1) {
                    perror("epoll_ctl failed");
                    close(client_fd);
                }
            }
            else{
                thread th(establish_session,event[i].data.fd );
                th.detach();
            }
        }
    }
    return 0;
}