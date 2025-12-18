#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSZ 1024

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    int sock;
    struct sockaddr_in srv_addr, cli_addr;
    char buf[BUFSZ];

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1){
        perror("Socket creating failed");
        exit(1);
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1){
        perror("Binding failed");
        close(sock);
        exit(1);
    }

    printf("Server started on port %s\n", argv[1]);

    while(1){
        socklen_t addrlen = sizeof(cli_addr);

        // cli_addr 和 addrlen 传参传的是指针，所以 recvfrom 可以实现将数据报的源地址及其长度被保存在这两个变量
        // 返回值是消息长度
        int msglen = recvfrom(sock, buf, BUFSZ - 1, 0, (struct sockaddr *)&cli_addr, &addrlen);
        if(msglen == -1){
            perror("Recvfrom failed");
            continue;
        }
        buf[msglen] = '\0';
        printf("Received request from: %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // 获取系统时间并转为字符串
        time_t now = time(NULL);
        char *time_str = ctime(&now);

        sendto(sock, time_str, strlen(time_str), 0, (struct sockaddr *)&cli_addr, addrlen);
    }

    close(sock);
    printf("Exited\n");
    return 0;
}
