#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>

enum CMDType {
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR,
};

//数据包头
struct DataHead {
	//数据长度
	short dataLength;
	//命令
	short cmd;
};

struct Login {
	char userName[32];
	char userPassWord[32];
};

struct LoginResult {
	int result;
};

struct Logout {
	char userName[32];
};

struct LogoutResult {
	int result;
};

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
	while (true) {
		memset(buf, 0, sizeof(buf));
		std::cin >> buf;
		if (strcmp(buf, "exit") == 0) {
			break;
		}
		else if (strcmp(buf, "login") == 0) {
			//向服务器发送登录指令
			Login login = { "Li heng", "1994218li" };
			DataHead head = { sizeof(Login), CMD_LOGIN };
			send(sock, (const char*)&head, sizeof(DataHead), 0);
			send(sock, (const char*)&login, sizeof(Login), 0);
			//接收来自服务器的消息
			LoginResult res = { };
			recv(sock, (char*)&head, sizeof(head), 0);
			recv(sock, (char*)&res, sizeof(res), 0);
			if (res.result == 0) {
				std::cout << "登录成功" << std::endl;
			}
			else {
				std::cout << "登录失败" << std::endl;
			}
		}
		else if (strcmp(buf, "logout") == 0) {
			//向服务器发送登出指令
			Logout logout = { "Li heng" };
			DataHead head = { sizeof(Logout), CMD_LOGOUT };
			send(sock, (const char*)&head, sizeof(DataHead), 0);
			send(sock, (const char*)&logout, sizeof(Logout), 0);
			//接收来自服务器的消息
			LogoutResult res = { };
			recv(sock, (char*)&head, sizeof(head), 0);
			recv(sock, (char*)&res, sizeof(res), 0);
			if (res.result == 1) {
				std::cout << "成功退出登录" << std::endl;
			}
			else {
				std::cout << "退出登录失败" << std::endl;
			}
		}
		else {
			std::cout << "不支持的命令，请重新输入\n";
		}
	}
	//关闭套接字
	closesocket(sock);
	WSACleanup();
	getchar();
	return 0;
}