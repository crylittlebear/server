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
	while (true) {
		SOCKET c_sock = accept(sock, (sockaddr*)&c_addr, &addr_len);
		if (c_sock == INVALID_SOCKET) {
			std::cerr << "接受客户端连接失败!\n";
			break;
		}
		else {
			std::cout << "成功接收到来自客户端的连接，IP: " << inet_ntoa(c_addr.sin_addr)
				<< ", port: " << ntohs(c_addr.sin_port) << std::endl;
			while (true) {
				DataHead head = { };
				int len = recv(c_sock, (char*)&head, sizeof(DataHead), 0);
				if (len > 0) {
					std::cout << "接收到数据长度: " << head.dataLength << ", 命令："
						<< head.cmd << std::endl;
					switch (head.cmd) {
					case CMD_LOGIN:
					{
						Login login = { };
						recv(c_sock, (char*)&login, sizeof(Login), 0);
						std::cout << "账户名: " << login.userName << ", 密码: " <<
							login.userPassWord << std::endl;
						std::cout << login.userName << "登录成功\n";
						LoginResult res = { 0 };
						head.dataLength = sizeof(LoginResult);
						send(c_sock, (const char*)&head, sizeof(DataHead), 0);
						send(c_sock, (const char*)&res, sizeof(LoginResult), 0);
						break;
					}
					case CMD_LOGOUT:
					{
						Logout logout = { };
						recv(c_sock, (char*)&logout, sizeof(Logout), 0);
						std::cout << "账户名: " << logout.userName << "发来退出登录请求"
							<< std::endl;
						LogoutResult res = { 1 };
						head.dataLength = sizeof(LogoutResult);
						send(c_sock, (const char*)&head, sizeof(DataHead), 0);
						send(c_sock, (const char*)&res, sizeof(LogoutResult), 0);
						std::cout << logout.userName << "退出登录成功\n";
						break;
					}
					default:
					{
						head.cmd = CMD_ERROR;
						head.dataLength = 0;
						std::cout << "无法解析传送来的指令" << std::endl;
						send(c_sock, (const char*)&head, sizeof(DataHead), 0);
					}
					}
				}
				else if (len == 0) {
					std::cout << "客户端断开了连接" << std::endl;
					break;
				}
				else {
					std::cout << "接收数据错误，任务结束\n";
					return -1;
				}
			}
		}
		//向客户端发送数据
		//send(c_sock, msg, strlen(msg) + 1, 0);
		closesocket(c_sock);
	}
	closesocket(sock);
	WSACleanup();

	//getchar();
	return 0;
}