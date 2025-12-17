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

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1){
        perror("Socket creating failed");
        exit(1);
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    srv_addr.sin_port = htons(atoi(argv[2]));

    printf("Input \".exit\"(without quotes) to exit, or anything else to check time\n\n");
    char *msg = "TIME";
    while(1){
        fgets(buf, BUFSZ, stdin);
        if(!strcmp(buf, ".exit\n")) break;

        sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

        socklen_t addrlen = sizeof(srv_addr);
        int msglen = recvfrom(sock, buf, BUFSZ - 1, 0, (struct sockaddr *)&srv_addr, &addrlen);
        if(msglen == -1){
            perror("Recvfrom failed");
            continue;
        }
        buf[msglen] = '\0';
        printf("Server time: %s\n", buf);
    }

    close(sock);
    printf("Exited\n");
    return 0;
}
