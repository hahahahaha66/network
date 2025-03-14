#include <cstdlib>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    //创建TCP套接字
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock==-1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //连接服务器
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORT);
    inet_pton(AF_INET,SERVER_IP,&server_addr.sin_addr);
    if(connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        perror("connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("connected to server!\n");

    //发艘消息并回显
    while(1){
        printf("enter message: ");
        fgets(buffer,BUFFER_SIZE,stdin);
        send(sock,buffer,strlen(buffer),0);
        memset(buffer,0,BUFFER_SIZE);
        ssize_t bytes_received=recv(sock,buffer,BUFFER_SIZE,0);
        if(bytes_received<=0){
            printf("server disconnected.\n");
            break;
        }
        printf("echo: %s",buffer);
    }

    //关闭连接
    close(sock);
    return 0;
}