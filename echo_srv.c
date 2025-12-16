#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUFSZ 1024

// 子进程处理客户端连接
void handle_client(int cli_sock) {
    char buf[BUFSZ];
    int len;
    while((len = recv(cli_sock, buf, BUFSZ - 1, 0))){
        send(cli_sock, buf, len, 0);
    }
    close(cli_sock);
    printf("Client connection closed\n");
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    int srv_sock, cli_sock;
    struct sockaddr_in srv_addr, cli_addr;
    socklen_t cli_addr_sz;
    pid_t pid;

    // 创建 socket
    srv_sock = socket(AF_INET, SOCK_STREAM, 0);     // IPv4，TCP 流式 socket，默认协议
    if(srv_sock == -1){
        perror("Socket creating failed");
        exit(1);
    }

    // 设置服务器地址
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;                  // IPv4
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // 绑定到本机所有网卡
    srv_addr.sin_port = htons(atoi(argv[1]));       // 端口号，网络字节序

    // 绑定 socket 到地址
    if(bind(srv_sock, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) == -1){
        perror("Binding failed");
        exit(1);
    }

    // 监听，请求队列最大长度为 5
    if(listen(srv_sock, 5) == -1){
        perror("Listening failed");
        exit(1);
    }
    printf("Server started, listening on port %s\n", argv[1]);

    // 防止僵尸进程
    signal(SIGCHLD, SIG_IGN);

    while(1){
        cli_addr_sz = sizeof(cli_addr);

        // accept 是阻塞调用
        cli_sock = accept(srv_sock, (struct sockaddr *)&cli_addr, &cli_addr_sz);
        if(cli_sock == -1){
            perror("Accepting failed");
            continue;
        }
        printf("New client connected: %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // 创建一个子进程去处理
        pid = fork();
        if(pid == -1){
            perror("Process creating failed");
            continue;
        }

        // 子进程的
        if(!pid){
            close(srv_sock);
            handle_client(cli_sock);
            return 0;
        }

        // 父进程的
        else close(cli_sock);
    }
    
    close(srv_sock);
    printf("Exited\n");
    return 0;
}
