#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>

//整数转换网络字节序（大小端的转换）

//主机字节序（数字）转换成网络字节序
uint16_t htons(uint16_t hostlong);
//网络字节序转换成主机字节序（数字）
uint16_t ntohs(uint16_t netshort);

//32位下主机字节序（数字）转换成网络字节序
uint32_t htonl(uint32_t hostlong);
//32位下网络字节序转换成主机字节序（数字）
uint32_t ntohl(uint32_t netlong);


//字符串转换网络字节序（点分十进制转换成大端数）

//字符串转换成网络字节序
int inet_pton(int af, const char *__restrict cp, void *__restrict buf);

//网络字节序转换字符串
//int inet_ntop(int af, const void *__restrict cp, char *__restrict buf, socklen_t len);


