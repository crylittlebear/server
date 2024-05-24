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
		std::cerr << "创建套接字失败!\n";
		return -1;
	}
	//绑定本地IP地址及端口号
	sockaddr_in addr = { };
	addr.sin_family = AF_INET;
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(9999);
	if (SOCKET_ERROR == bind(sock, (sockaddr*)&addr, sizeof(addr))) {
		std::cerr << "绑定本地IP和端口失败!\n";
		return -1;
	}
	else {
		std::cout << "绑定IP和端口成功!\n";
	}
	//监听客户端连接
	if (SOCKET_ERROR == (listen(sock, 5))) {
		std::cerr << "监听失败\n";
		return -1;
	}
	else {
		std::cout << "监听中....\n";
	}
	//接受来自客户端的连接
	sockaddr_in c_addr = {};
	int addr_len = sizeof(c_addr);
	const char msg[] = "客户端你好，我是服务器!";
	while (true) {
		SOCKET c_sock = accept(sock, (sockaddr*)&c_addr, &addr_len);
		if (c_sock == INVALID_SOCKET) {
			std::cerr << "接受客户端连接失败!\n";
			return -1;
		}
		else {
			std::cout << "成功接收到来自客户端的连接，IP: " << inet_ntoa(c_addr.sin_addr)
				<< ", port: " << ntohs(c_addr.sin_port) << std::endl;
		}
		//向客户端发送数据
		send(c_sock, msg, strlen(msg) + 1, 0);
		closesocket(c_sock);
	}
	closesocket(sock);
	WSACleanup();

	//getchar();
	return 0;
}