#include "../include/server.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
 
void server::set_unlocking(int fd) {
    int flags = fcntl(fd,F_GETFL,0);
    if(fcntl(fd,F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl failed");
    }
}

server::server() {
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
        return ;
    }

    sock_addr.sin_family=AF_INET;
    sock_addr.sin_port=htons(PORT);
    inet_pton(AF_INET,IP,&sock_addr.sin_addr);

    int bind_return = bind(socket_fd,(sockaddr* )&sock_addr,sizeof(sock_addr));
    if(bind_return == -1){
        perror("bind failed");
        exit(EXIT_FAILURE);
        return ;
    }

    epfd = epoll_create(1);

}

void server::establishing_session() {
    while(1){

        int cliend_file = 0;
        char buffer[1024];
        int bytes = recv(cliend_fd,buffer,sizeof(buffer),0);

        if(bytes > 0) {
            send(cliend_fd,buffer,bytes,0);
            buffer[bytes]='\0';
            if(strcmp(buffer,"PASV") == 0) {
                future<int> result = async(launch::async,&server::establishing_data_connection,this);
                cliend_file = result.get();
            }
            else if(strcmp(buffer,"LIST") == 0) {

            }
            else if(strcmp(buffer,"STOR") == 0) {
                
            }
            else if(strcmp(buffer,"RETR") == 0) {
                
            }
            else {
                cout<<buffer<<endl;
            }
        }
        else if(bytes == -1 && errno == EAGAIN) {
            break;
        }
        else if(bytes == -1 && errno != EAGAIN) {
            cout<<"a error occured ..."<<endl<<"actively disconnect"<<endl;
            close(cliend_fd);
            break;
        }
        else if(bytes == 0) {
            cout<<"client disconnect"<<endl;
            close(cliend_fd);
            break;
        }
    }
}

int server::establishing_data_connection() {
    int file_fd = socket(AF_INET,SOCK_STREAM,0);
    if(file_fd == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    char pasv_resp[64];
    struct sockaddr_in file_addr;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(10000,50000);

    

    int file_port=0;
    do {file_port = dis(gen);

        memset(&file_addr, 0, sizeof(file_addr));

        cout<<file_port<<endl;
        file_addr.sin_family=AF_INET;
        inet_pton(AF_INET,IP,&file_addr.sin_addr);
        file_addr.sin_port=htons(file_port);
     
    } while((bind(file_fd,(sockaddr*)&file_addr,sizeof(file_addr)) == -1));
    

    //snprintf(pasv_resp, sizeof(pasv_resp), "227 entering passive mode (127,0,0,1,%d,%d)\r\n",file_port/256,file_port%256);
    snprintf(pasv_resp, sizeof(pasv_resp), "%d %d",file_port/256,file_port%256);
    cout<<pasv_resp<<endl;
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

    cout<<"The data connection channel was created successfully"<<endl;

    return cliend_file;
}

server::~server() {
    close(socket_fd);
}
void server::server_accept_with_comminicate() {
    if(listen(socket_fd,10) == -1){
        perror("listen failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    set_unlocking(socket_fd);
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epfd,EPOLL_CTL_ADD,socket_fd,&ev) == -1) {
        perror("epoll_ctl failed");
    }

     while(1){
        int nfds = epoll_wait(epfd,event,MAX_EVENT,-1);
        for(int i = 0;i < nfds; i++){

            if(event[i].data.fd == socket_fd) {
                int client_fd = accept(socket_fd,NULL,NULL);
                if(client_fd == -1) {
                    perror("accept failed");
                    continue;
                }
                else {
                    cout<<"cliend_fd : "<<client_fd<<" connected"<<endl;
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
                int temp=event[i].data.fd;
                thread th(&server::establishing_session,this,temp);
                th.detach();
            }
        }
    }
}

int main() {
    server haha;
    haha.server_accept_with_comminicate();
    return 0;
}