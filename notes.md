### Socket
每一种socket都至少提供了两种socket,分别是流和数据报
数据报使用了UDP协议，流使用TCP协议，各有优缺点
| 属性           | 流     | 数据报 |
|--------------|------|------|
| 可靠的传送   | 是   | 否   |
| 消息边界保留 | 否   | 是   |
| 面向连接     | 是   | 否   |

![工作流程](./1.png)

#### socket
    #include <sys/types.h>
    #include <sys/socket.h>

    int socket(int domain, int type, int protocol);
domain,指定通信协议族，常见的协议族有以下
|值           |含义          |
|-------------|--------------|
|AF_INET      |IPV4网络协议  |
|AF_INET6     |IPV6网络协议  |
|AF_UNIX      |本地进程间通信|
|AF_PACKET    |访问底层网络  |

tpye,表示套接字的传输方式，常见的有
|值              |含义              |
|----------------|------------------|
|SOCK_STREAM     |TCP               |
|SOCK_DGRAM      |UDP               |
|SOCK_RAM        |原始套接字        |
|SOCK_SEQPACKET  |序列包            |

protocol通常为0
socket返回值为新建的socket的套接字描述符，返回值为-1时表示失败
#### bind
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
sockfd就是socket的返回值，套接字描述符

addr是指向结构体sockaddr的指针，sockaddr具体为
    struct{
        sa_family_t sa_family;
        char sa_date[14];
    }
主要用来将各种domain转化为一个统一的结构

addrlen指定结构体的大小

#### listen
    #include<sys/socket.h>

    int liste(int sockfd,int backlog);
sockfd是由socket创建的并返回的套接字描述符，必须是SOCK_STREAM类型的TCP套接字

backlog控制等待连接的队列的大小，设定已完成连接的队列的最大长度

成功返回0,失败返回-1

#### accept
    #include <sys/socket.h>

    int accept(int sockfd,struct sockaddr* addr,socklen_t*addrlen);
与bind类似，参数基本相同，addr用于存放客户端地址，addrlen表示结构体的大小

注意，accept是服务端接受客户端连接的，如果未存在连接，就会一直堵塞到有连接为止

成功返回新的socket,失败返回-1

原来的socket是监听套接字，只能用于连接，新的socket是专门用于通信的

#### connect
    #include <sys/socket.h>
    int connect(int sockfd, const stuct socksddr *addr,socklen_t addrlen);
参数基本与accept相同，功能也类似

注意，当客户端连接服务端失败后，需要重新建立一个socket来再次尝试连接

#### close
终止连接，可以将引用了socket的文件描述符都用close关闭，连接就会终止

