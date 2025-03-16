#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

int main() {
    int sv[2];  // 存储一对 socket 描述符
    char buf[100];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        return 1;
    }

    if (fork() == 0) {  // 子进程
        close(sv[0]);  // 关闭未使用的 socket
        //write(sv[1], "Hello, Parent!\n", 15);
        write(sv[1],"hahahahahaha", 13);
        close(sv[1]);
    } else {  // 父进程
        close(sv[1]);  // 关闭未使用的 socket
        read(sv[0], buf, sizeof(buf));
        printf("Son send: %s\n", buf);
        close(sv[0]);
    }

    return 0;
}
