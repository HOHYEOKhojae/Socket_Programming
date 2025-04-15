// To use socket, refer to library
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>

// Socket library
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Buffer size
#define BUFFERSIZE 1024
using namespace std;

// Header
#include "Rxdata.h"

// 접속되는 클라이언트별 쓰레드
void client(int clientSock, struct sockaddr_in clientAddr, vector<thread*>* clientlist) {
    // IP 주소 문자열로 변환
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ip_str, INET_ADDRSTRLEN);
    cout << "Client connected IP address = " << ip_str << ":" << ntohs(clientAddr.sin_port) << endl;

    const char* message = "Welcome server!\r\n>\0";
    send(clientSock, message, strlen(message) + 1, 0);

    vector<char> buffer;
    char x;
    while (1) {
        if (recv(clientSock, &x, sizeof(char), 0) <= 0) {
            cout << "error or connection closed" << endl;
            break;
        }
        if (buffer.size() > 0 && *(buffer.end() - 1) == '\r' && x == '\n') {
            if (*buffer.begin() == 'e' && *(buffer.begin() + 1) == 'x' && *(buffer.begin() + 2) == 'i' && *(buffer.begin() + 3) == 't') {
                break;
            }
            const char* echo = print(&buffer);
            send(clientSock, echo, buffer.size() + 10, 0);
            delete[] echo;
            buffer.clear();
            continue;
        }
        buffer.push_back(x);
    }

    close(clientSock);
    cout << "Client disconnected IP address = " << ip_str << ":" << ntohs(clientAddr.sin_port) << endl;

    for (auto ptr = clientlist->begin(); ptr < clientlist->end(); ptr++) {
        if ((*ptr)->get_id() == this_thread::get_id()) {
            clientlist->erase(ptr);
            break;
        }
    }
}

// 실행 함수
int main() {
    vector<thread*> clientlist;

    // 소켓 생성
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        cout << "socket error" << endl;
        return 1;
    }

    // 소켓 주소 설정
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9090);

    // 소켓 바인딩
    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "bind error" << endl;
        close(serverSock);
        return 1;
    }

    // 소켓 대기 상태
    if (listen(serverSock, SOMAXCONN) < 0) {
        cout << "listen error" << endl;
        close(serverSock);
        return 1;
    }

    cout << "Server Start" << endl;

    // 클라이언트 접속 대기
    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);
        int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &len);
        if (clientSock < 0) {
            cout << "accept error" << endl;
            continue;
        }
        clientlist.push_back(new thread(client, clientSock, clientAddr, &clientlist));
    }

    // 쓰레드 종료 대기
    for (auto ptr = clientlist.begin(); ptr < clientlist.end(); ptr++) {
        (*ptr)->join();
        delete *ptr;
    }

    close(serverSock);
    return 0;
}