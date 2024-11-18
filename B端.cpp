#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
	WSADATA wsaData;
	SOCKET clientSocket;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	int bytesRead;
	std::string serverIP;
	int port;

	// 初始化 Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup 失败" << std::endl;
		return 1;
	}

	// 创建套接字
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "创建套接字失败" << std::endl;
		WSACleanup();
		return 1;
	}

	// 手动输入服务器IP地址和端口
	std::cout << "请输入服务器IP地址: ";
	std::cin >> serverIP;
	std::cout << "请输入服务器端口: ";
	std::cin >> port;
	std::cin.ignore(); // 忽略换行符

	// 设置服务器地址
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddr.sin_addr)) <= 0) {
		std::cerr << "无效的地址/地址不受支持" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// 连接服务器
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "连接服务器失败" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "连接到服务器成功" << std::endl;

	// 聊天循环
	while (true) {
		// 发送消息给服务器
		std::string sendMessage;
		std::cout << "客户端: ";
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

		// 接收服务器消息
		int messageLength;
		bytesRead = recv(clientSocket, (char*)&messageLength, sizeof(messageLength), 0);
		if (bytesRead <= 0) {
			std::cerr << "服务器断开连接" << std::endl;
			break;
		}

		bytesRead = recv(clientSocket, buffer, messageLength, 0);
		if (bytesRead <= 0) {
			std::cerr << "服务器断开连接" << std::endl;
			break;
		}

		buffer[bytesRead] = '\\0'; // 确保字符串以 null 结尾
		std::string receivedMessage(buffer, messageLength);
		std::cout << "服务器: " << receivedMessage << std::endl;
	}

	// 关闭套接字
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
