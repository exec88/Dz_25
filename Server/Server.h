#pragma once

//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream> 
#include <string>
//#include <cstdio>
//#include <cstring> 
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")

constexpr int messageLen = 1024;

class Server
{
public:
	Server() {}

	~Server() {}

	bool initSock(std::string ip, uint16_t port) {
		
		_ip = ip;
		_port = port;

		WSADATA WSAData; //Данные 
		WORD ver = MAKEWORD(2, 2);

		int wsInit = WSAStartup(MAKEWORD(2, 2), &WSAData);

		if (wsInit != 0) {
			std::cout << "Error: can't initialize Winsock." << std::endl;
			return false;
		}
		return true;
	}

	void runServer() {

		server = socket(AF_INET, SOCK_STREAM, 0); //Создали сервер

		if (server == INVALID_SOCKET) {
			std::cout << "Socket creation failed with error:" << WSAGetLastError() << std::endl;
			return;
		}
		SOCKADDR_IN serverAddr, clientAddr; //Адреса сокетов
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(_port);

		if (bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cout << "Bind function failed with error: " << WSAGetLastError() << std::endl;
			return;
		}

		if (listen(server, 0) == SOCKET_ERROR) { //Если не удалось получить запрос
			std::cout << "Listen function failed with error:" << WSAGetLastError() << std::endl;
			return;
		}
		std::cout << "Listening for incoming connections...." << std::endl;

		int clientAddrSize = sizeof(clientAddr); //Инициализировать адерс клиента
		if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
				//Если соединение установлено
				std::cout << "Client connected!" << std::endl;
		}
	}

	void closeServer() {
		closesocket(client); //Закрыть сокет

		if (closesocket(server) == SOCKET_ERROR) { //Ошибка закрытия сокета
			std::cout << "Close socket failed with error: " << WSAGetLastError() << std::endl;
			return;
		}
		WSACleanup();
	}

	std::string readMessage() {
		message.clear();
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

private:
	std::string _ip{};
	uint16_t _port{};
	SOCKET server, client; //Сокеты сервера и клиента
	char buffer[messageLen] = { 0 }; //Буфер для данных
	std::string message{};
};