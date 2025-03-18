#include <bits/types/struct_iovec.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
//从sockfd接收数据
ssize_t recv(int sockfd,void *buffer,size_t lenght,int flags);
//向sockfd发送数据
ssize_t send(int fd, const void *buf, size_t n, int flags);
//提高从磁盘上传文件的效率
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
//out_fd为套接字描述符
//in_fd为要传输的文件描述符
//off_t传输文件偏移量，NULL则从头开始传输
//count制定传输字节大小
