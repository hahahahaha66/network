#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main(){
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(8080);
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr))<0){
        perror("bind failed");
        return 1;
    }
    return 0;
}