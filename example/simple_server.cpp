#include <asm-generic/socket.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    int server_fd,client_fd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t addr_len=sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    //创建套接字
    server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd==-1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //绑定ip和端口
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    server_addr.sin_port=htons(PORT);
    if(bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //监听端口
    if(listen(server_fd,5)==-1){
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("server listening on port %d ...\n",PORT);
    
    //连接客户端
    client_fd=accept(server_fd,(struct sockaddr*)&client_addr,&addr_len);
    if(client_fd==-1){
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("client connect!\n");

    //读取客户端消息并回显
    while(1){
        memset(buffer,0,BUFFER_SIZE);
        ssize_t bytes_received=recv(client_fd,buffer,BUFFER_SIZE,0);
        if(bytes_received<=0){
            printf("client disconnected.\n");
            break;
        }
        printf("received: %s",buffer);
        send(client_fd,buffer,bytes_received,0);
    }
    
    //关闭链接
    close(server_fd);
    close(client_fd);
    return 0;
}