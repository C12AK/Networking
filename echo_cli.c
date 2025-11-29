#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSZ 1024

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(1);
    }

    int sock;
    struct sockaddr_in srv_addr;
    char buf[BUFSZ];
    int len;

    // 创建 socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        perror("Socket creating failed");
        exit(1);
    }

    // 设置服务器地址
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(argv[1]);      // 将 char *IP 转为二进制
    srv_addr.sin_port = htons(atoi(argv[2]));

    // 连接到服务器
    if(connect(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1){
        perror("Connecting failed");
        exit(1);
    }
    printf("Connected to server %s:%s\n", argv[1], argv[2]);
    printf("Input \".exit\"(without quotes) to exit\n\n");

    while(1){
        printf("Message to send: ");
        fgets(buf, BUFSZ, stdin);
        if(!strcmp(buf, ".exit\n")) break;
        write(sock, buf, strlen(buf));
        len = read(sock, buf, BUFSZ - 1);
        if(len == -1){
            perror("Receiving failed");
            break;
        }
        buf[len] = '\0';
        printf("Server returned: %s\n", buf);
    }

    close(sock);
    printf("Exited\n");
    return 0;
}
