#pragma once

#include "Client.h"

#include <string>
#include <iostream>

class Chat final {
public:
	Chat(const std::string& ip, const uint16_t port) :
		_ip(ip), _port(port), _connection(false), _isUserOnline(false) {

		if (_client.initSock(_ip, _port)) _client.runClient();
	}

	void run() {
		_connection = true;
		//int operation;
		std::string message{};
		std::cout << "\n*** Welcome to Console Chat! ***" << std::endl;
		while (_connection)
		{
			// меню регистрации или авторизации пользователя
			if (!_isUserOnline) {
				std::cout << " \n1 - Autorization \n2 - Registration \n0 - Exit" << std::endl;
			}
			else {
				std::cout << "\n*** You have entered the User menu ***" << std::endl;
				std::cout << "\n3 - Show all users \n4 - Show all messages"
					<< "\n5 - Add message to chat \n6 - Change password \n7 - Sign Out" << std::endl;
			}

			int operation{};
			std::cout << "User: " << std::endl;
			//std::cin.ignore();
			std::cin >> operation;
			_client.sendMessage(std::to_string(operation));
			message = _client.readMessage();
			std::cout << "Server: " << message << std::endl;
			if (message[0] == '1') {
				std::string name, login, password;
				int usersCount{};
				bool loginOk = false, passwordOk = false;
				std::string to, text;
				int messagesCount{};
				std::string newPassword{};

				switch (operation)
				{
				case 1:
					std::cout << "\n\t***LOGIN***" << std::endl;
					loginOk = false;
					passwordOk = false;
					while (!loginOk) {
						std::cout << "Enter your login: " << std::endl;
						std::cin >> login;
						_client.sendMessage(login);
						if (_client.readMessage() == "1") {
							loginOk = true;
							while (!passwordOk) {
								std::cout << "Enter your password: " << std::endl;
								std::cin >> password;
								_client.sendMessage(password);
								if (_client.readMessage() == "1") {
									std::cout << "\n*Hello " << login << "!*" << std::endl;
									_currentUser = login;
									_isUserOnline = true;
									passwordOk = true;
								}
								else
									std::cout << "The password entered is incorrect, try again!" << std::endl;
							}
						}
						else
							std::cout << "The login entered is incorrect, try again!" << std::endl;
					}
					break;
				case 2:
					std::cout << "\n\t***SIGNUP***" << std::endl;
					std::cout << "Enter your name: " << std::endl;
					std::cin >> name;
					_client.sendMessage(name);
					while (!passwordOk) {
						std::cout << "Enter your login: " << std::endl;
						std::cin >> login;
						_client.sendMessage(login);
						if (_client.readMessage() == "0") {
							std::cout << "Enter your password: " << std::endl;
							std::cin >> password;
							_client.sendMessage(password);
							passwordOk = true;
						}
						else
							std::cout << "Login is bisy, try again!" << std::endl;
					}
					std::cout << "Server: " << _client.readMessage() << std::endl;
					_isUserOnline = true;
					_currentUser = login;
					break;
				case 3:
					std::cout << "\n\t***SHOW ALL USERS IN CHAT***" << std::endl;
					usersCount = std::stoi(_client.readMessage());
					_client.sendMessage(" ");
					std::cout << "Count of users: " <<  usersCount << std::endl;
					std::cout << "Logins:\n";
					for (int i = 0; i < usersCount; i++) {
						login = _client.readMessage();
						_client.sendMessage(" ");
						if(login == _currentUser)
							std::cout << login  << " (You)" << std::endl;
						else
							std::cout << login << std::endl;
					}
					break;
				case 4:
					std::cout << "\n\t***SHOW ALL MESSAGES IN CHAT***" << std::endl;
					std::cout << " \n1 - Show messages for all users \n2 - Show private messages" << std::endl;
					std::cout << _currentUser << ": " << std::endl;
					std::cin.ignore();
					std::cin >> operation;
					_client.sendMessage(std::to_string(operation));
					switch (operation)
					{
					case 1:
						messagesCount = std::stoi(_client.readMessage());
						if (messagesCount == 0)
							std::cout << "It's no messages to all in this chat!" << std::endl;
						else {
							std::cout << "Messages to all: " << messagesCount << std::endl;
							_client.sendMessage(" ");
							for (int i = 0; i < messagesCount; i++) {
								std::cout << _client.readMessage() << std::endl;
								_client.sendMessage(" ");
							}
						}
						break;
					case 2:
						messagesCount = std::stoi(_client.readMessage());
						if (messagesCount == 0)
							std::cout << "It's no messages to You, " << _currentUser << std::endl;
						else {
							std::cout << "Messages to You: " << messagesCount << std::endl;
							_client.sendMessage(" ");
							for (int i = 0; i < messagesCount; i++) {
								std::cout << _client.readMessage() << std::endl;
								_client.sendMessage(" ");
							}
						}
						break;
					default:
						break;
					}
					break;
				case 5:
					std::cout << "\n\t***ADD MESSAGE TO CHAT***" << std::endl;
					loginOk = false;
					while (!loginOk) {
						std::cout << "To (Login of user or to \"all\" users): " << std::endl;
						std::cin >> to;
						_client.sendMessage(to);
						if (!(to == "all")) {
							if (_client.readMessage() == "1") {
								loginOk = true;
								std::cout << "Enter the text:" << std::endl;
								std::cin.ignore();
								std::getline(std::cin, text);
								_client.sendMessage(text);
							}
							else
								std::cout << "Can't find user with name " << to << std::endl;
						}
						else {
							loginOk = true;
							std::cout << "Enter the text:" << std::endl;
							std::cin.ignore();
							std::getline(std::cin, text);
							_client.sendMessage(text);
						}
					}
					break;
				case 6:
					std::cout << "\n\t***CHANGE PASSWORD***" << std::endl;
					std::cout << "Enter new password:" << std::endl;
					std::cin >> newPassword;
					_client.sendMessage(newPassword);
					if (_client.readMessage() == "1") {
						std::cout << "The password was successfully changed" << std::endl;
					}
					else {
						std::cout << "Error! The password was not changed" << std::endl;
					}
					break;
				case 7:
					std::cout << "\n\t***SIGN OUT***" << std::endl;
					std::cout << "User offline" << std::endl;
					_isUserOnline = false;
					break;
				case 0:
					std::cout << "\n\t***EXIT***" << std::endl;
					std::cout << "Thank you for use Console Chat! Good Bye!" << std::endl;
					_connection = false;
					_client.closeClient();
					break;
				default:
					break;
				}
			}
		}
	}

private:
	Client _client;			
	std::string _ip;		/** IP сервера */
	uint16_t _port;			/** номер порта сервера */
	bool _connection;
	bool _isUserOnline;			/** признак наличия пользователя в чате */
	std::string _currentUser;	/** имя текущего пользователя, находящегося в чате */
};