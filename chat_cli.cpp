#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
using namespace std;

#define BUFSZ 1024
char buf[BUFSZ];
string msg;

int main(int argc, char *argv[]){
    if(argc != 3){
        cerr << format("Usage: {} <Server IP> <Port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        cerr << "Socket creating failed.\n";
        exit(1);
    }

    sockaddr_in srv_addr{};
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    srv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (sockaddr *)&srv_addr, sizeof(srv_addr)) < 0){
        cerr << "Connecting to server failed.\n";
        exit(1);
    }
    cout << "Connected to server.\n";

    fd_set fds;
    int mxfd = max(sock, fileno(stdin));

    while(1){
        // 重新初始化可读事件的文件描述符集合，应包含服务器消息和键盘输入
        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        FD_SET(fileno(stdin), &fds);

        // 内核检查的范围是 [0, mxfd+1)；返回值是就绪（即变为可读）的文件描述符数量
        int ready = select(mxfd + 1, &fds, nullptr, nullptr, nullptr);
        if(ready < 0){
            cerr << "Select error.\n";
            break;
        }

        // 如果是有服务器消息
        if(FD_ISSET(sock, &fds)){
            int len = recv(sock, buf, BUFSZ - 1, 0);
            if(len < 0){
                cerr << "Recv error.\n";
                break;
            }
            buf[len] = '\0';
            cout << format("\n> Received:\n> {}\n", buf) << endl;
        }

        // 如果是键盘有输入
        if(FD_ISSET(fileno(stdin), &fds)){
            if(!getline(cin, msg) || msg == ".exit") break;
            send(sock, msg.c_str(), msg.size(), 0);
        }
    }

    close(sock);
    return 0;
}
