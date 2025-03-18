#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

const char* IP="192.168.1.1";
const int PORT=2100;
 
// int get_next_file_name () {
//     DIR* dir=opendir("file");
//     if(!dir) {
//         return 1;
//     }
//     struct dirent *entry;
//     int num=0;
//     while((entry=readdir(dir))){
//         int i;
//         if(sscanf(entry->d_name,"file_%d.txt",&num)==1){
            
//         }
//     }
// }
void accetp_data (int cliend_fd) {
    char buffer[1024];
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
    sock_addr.sin_addr.s_addr=inet_pton(AF_INET,IP,&IP);
    int bind_return = bind(socket_fd,(sockaddr* )&sock_addr,sizeof(sock_addr));
    if(bind_return == -1){
        perror("bind failed");
        exit(EXIT_FAILURE);
        return 1;
    }

    listen(socket_fd,10);

    while(1){
        int* cliend_fd = new int;
        *cliend_fd = accept(socket_fd,NULL,NULL);
        thread t1(accetp_data,cliend_fd);
        t1.join();
    }
    
    return 0;
}