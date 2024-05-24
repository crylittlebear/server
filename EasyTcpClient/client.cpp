#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>

int main() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//创建套接字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cerr << "创建套接字失败\n";
		return -1;
	}
	else {
		std::cout << "创建套接字成功\n";
	}
	//连接服务器
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9999);
	if (SOCKET_ERROR == connect(sock, (sockaddr*)&addr, sizeof(addr))) {
		std::cout << "连接服务器失败\n";
		return -1;
	}
	else {
		std::cout << "连接服务器成功\n";
	}
	//接收消息
	char buf[1024];
	recv(sock, buf, sizeof(buf), 0);
	std::cout << "服务器说: " << buf << std::endl;
	//关闭套接字
	closesocket(sock);
	WSACleanup();
	getchar();
	return 0;
}