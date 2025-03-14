#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//node 域名或ip地址
//service 服务名或端口字符串
//hints 指定查询条件
//res 存放查询结果的的指针
int getaddrinfo(const char *node, const char *service,const struct addrinfo *hints,struct addrinfo **res);
//主要用于解析域名为ip地址，便于socket操作
//struct addrinfo
// {
//   int ai_flags;			/* Input flags.  */
//   int ai_family;		/* Protocol family for socket.  */
//   int ai_socktype;		/* Socket type.  */
//   int ai_protocol;		/* Protocol for socket.  */
//   socklen_t ai_addrlen;		/* Length of socket address.  */
//   struct sockaddr *ai_addr;	/* Socket address for socket.  */
//   char *ai_canonname;		/* Canonical name for service location.  */
//   struct addrinfo *ai_next;	/* Pointer to next in list.  */
// };
//使用freeaddrinfo释放addrinfo链表
//使用gai_strerror来获取错误诊断

//getaddrinfo的逆向操作，将socket转换为可读的主机名或服务名
int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags);
//sa 指向要转换的sockaddr结构体的指针
//salen sa结构体的大小
//host 存储解析的主机名
//hostlen 缓冲区的大小
//serv 存储解析出的端口的缓冲区指针
//servlen 缓冲区的大小
//flags 控制解析选项
int main(){
    char* node;
    char* service;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;
    int status;
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    status=getaddrinfo("google.com", "http",&hints, &res);
    if(status!=0){
        return 1;
    }
    for(p=res;p!=NULL;p=p->ai_next){
        char ipstr[INET6_ADDRSTRLEN];
        void* addr;

        if(p->ai_family==AF_INET){
            struct sockaddr_in *ipv4=(struct sockaddr_in*)p->ai_addr;
            addr=&(ipv4->sin_addr);
        }
        if(p->ai_family==AF_INET6){
            struct sockaddr_in6 *ipv6=(struct sockaddr_in6*)p->ai_addr;
            addr=&(ipv6->sin6_addr);
        }

        inet_ntop(p->ai_family,addr,ipstr,sizeof(ipstr));
        printf("IP:%s\n",ipstr);
    }

    freeaddrinfo(res);
    return 0;
}