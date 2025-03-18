#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
int main(){
    struct in_addr ipv4_addr;
    ipv4_addr.s_addr=htonl(0xC0A80101);

    char ipv4_str[INET_ADDRSTRLEN];

    //将网络二进制ip地址转换为网络字节序
    if(inet_ntop(AF_INET,&ipv4_addr,ipv4_str,INET_ADDRSTRLEN)==NULL){
        perror("inet_ntop");
        return 1;
    }

    printf("IPV4 address: %s\n",ipv4_str);

    //将展现字符串转换为网络字节序的二进制ip地址
    if(inet_pton(AF_INET,ipv4_str,&ipv4_addr)<=0){
        perror("inet_pton");
        return 1;
    }

    printf("ipv4 address is 0x%x \n",ntohl(ipv4_addr.s_addr));

    return 0;
}