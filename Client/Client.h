#pragma once

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream> 
#include <string>
//#include <cstdio>
//#include <cstring> 
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")

constexpr int messageLen = 1024;

class Client
{
public:
	Client() {}

	~Client() {}

	bool initSock(std::string ip, int port) {

		_ip = ip;
		_port = port;

		WSADATA WSAData; //Данные 
		int wsInit = WSAStartup(MAKEWORD(2, 2), &WSAData);

		if (wsInit != 0) {
			std::cout << "Error: can't initialize Winsock." << std::endl;
			return false;
		}
		return true;
	}

	void runClient() {

		client = socket(AF_INET, SOCK_STREAM, 0); //Создали клиент

		if (client == INVALID_SOCKET) {
			std::cout << "Socket creation failed with error:" << WSAGetLastError() << std::endl;
			return;
		}

		SOCKADDR_IN clientAddr; //Адрес сокета
		clientAddr.sin_addr.s_addr = inet_addr(_ip.c_str()); //коннект к серверу
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(_port); //порт
		if (connect(client, (SOCKADDR*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
			std::cout << "Server connection failed with error: " << WSAGetLastError() << std::endl;
			return;
		}

		std::cout << "Connected to server!" << std::endl;
	}

	void closeClient() {
		closesocket(client); //Закрыть сокет
		WSACleanup();
	}

	std::string readMessage() {
		if (recv(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
			//Если не удалось получить данные буфера, сообщить об ошибке и выйти
			std::cout << "recv function failed with error " << WSAGetLastError() << std::endl;
			return "";
		}
		message = buffer;
		memset(buffer, 0, sizeof(buffer));
		return message;
	}

	void sendMessage(const std::string& mes) {
		if (send(client, mes.c_str(), sizeof(mes), 0) == SOCKET_ERROR) {
			std::cout << "send failed with error " << WSAGetLastError() << std::endl;
			return;
		}
	}

	//void loop() {
	//	while (true)
	//	{
	//		std::cout << "Client: ";
	//		getline(std::cin, message);
	//		sendMessage(message);
	//		if (message == "exit") {
	//			std::cout << "Thank you for using the application" << std::endl;
	//			break;
	//		}
	//		message = readMessage();
	//		std::cout << "Server: " << message << std::endl;
	//		if (message == "exit") {
	//			std::cout << "Server Disconnected." << std::endl;
	//			break;
	//		}
	//	}
	//	closeClient();
	//}

private:
	std::string _ip{};
	int _port{};
	SOCKET client; //Сокет клиента
	char buffer[messageLen] = { 0 }; //Буфер для данных
	std::string message{};
};