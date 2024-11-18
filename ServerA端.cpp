#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
	WSADATA wsaData;
	SOCKET serverSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	char buffer[1024];
	int bytesRead;
	int clientAddrSize = sizeof(clientAddr);
	int port;

	// 初始化 Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup 失败" << std::endl;
		return 1;
	}

	// 创建套接字
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "创建套接字失败" << std::endl;
		WSACleanup();
		return 1;
	}

	// 手动设置端口
	std::cout << "请输入服务器端口: ";
	std::cin >> port;
	std::cin.ignore(); // 忽略换行符

	// 设置服务器地址
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// 绑定套接字
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "绑定套接字失败" << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	// 监听连接
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "监听失败" << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "服务器正在监听端口 " << port << "..." << std::endl;

	// 接受客户端连接
	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "接受客户端连接失败" << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "客户端已连接" << std::endl;

	// 聊天循环
	while (true) {
		// 接收客户端消息
		int messageLength;
		bytesRead = recv(clientSocket, (char*)&messageLength, sizeof(messageLength), 0);
		if (bytesRead <= 0) {
			std::cerr << "客户端断开连接" << std::endl;
			break;
		}

		bytesRead = recv(clientSocket, buffer, messageLength, 0);
		if (bytesRead <= 0) {
			std::cerr << "客户端断开连接" << std::endl;
			break;
		}

		buffer[bytesRead] = '\\0'; // 确保字符串以 null 结尾
		std::string receivedMessage(buffer, messageLength);
		std::cout << "客户端: " << receivedMessage << std::endl;

		// 发送消息给客户端
		std::string sendMessage;
		std::cout << "服务器: ";
		std::getline(std::cin, sendMessage);
		int sendMessageLength = sendMessage.size();

		// 先发送消息长度
		if (send(clientSocket, (char*)&sendMessageLength, sizeof(sendMessageLength), 0) == SOCKET_ERROR) {
			std::cerr << "发送消息长度失败" << std::endl;
			break;
		}

		// 再发送消息内容
		if (send(clientSocket, sendMessage.c_str(), sendMessageLength, 0) == SOCKET_ERROR) {
			std::cerr << "发送消息失败" << std::endl;
			break;
		}
	}

	// 关闭套接字
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}
