#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_EVENTS 10

void set_noblocking(int fd){
    int flags = fcntl(fd,F_GETFL,0);
    fcntl(fd,F_SETFL,flags | O_NONBLOCK);
}

int main(){
    int epfd = epoll_create(1);
    struct epoll_event ev, events[MAX_EVENTS];

    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_addr = {AF_INET,htons(PORT),INADDR_ANY};
    int i = bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(i == -1){
        perror("bind failed");
        exit(EXIT_FAILURE);
        return -1;
    }
    listen (server_fd,5);

    set_noblocking(server_fd);
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,server_fd,&ev);

    while (1) {
        int nfds = epoll_wait(epfd,events,MAX_EVENTS,-1);
        for(int i = 0 ; i < nfds; i++) {
            if(events[i].data.fd == server_fd) {
                int client_fd = accept(server_fd,NULL,NULL);
                set_noblocking(client_fd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&ev);
            }
            else {
                char buffer[1024];
                int bytes = recv(events[i].data.fd,buffer,sizeof(buffer),0);
                if (bytes > 0) {
                    send(events[i].data.fd,buffer,bytes,0);
                }
                else {
                    close(events[i].data.fd);
                }

            }
        }
    }

    return 0;
}