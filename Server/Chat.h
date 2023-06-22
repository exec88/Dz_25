#pragma once

#include "Database.h"
#include "Server.h"

#include <fstream>
#include <string>
//#include <vector>

class Chat final {
public:
	Chat(const std::string& ip, const uint16_t port,
		const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword, const std::string& dbName) : 
		_isUserOnline(false), _connection(false), _ip(ip), _port(port),
		_dbHost(dbHost), _dbUser(dbUser), _dbPassword(dbPassword), _dbName(dbName) {
		
		_dataBase.connect(_dbHost, _dbUser, _dbPassword, _dbName);
		if (_server.initSock(_ip, _port)) _server.runServer();
	}
	
	~Chat() = default;

	void run() {
		_connection = true;
		while (_connection)
		{
			std::string message{};
			message = _server.readMessage();
			std::string name, login, password;
			std::vector<std::string> users{};
			bool loginOk = false, passwordOk = false;
			std::string to, text;
			std::vector<std::string> userMessages;
			std::string newPassword{};

			std::cout << "User: " << message << std::endl;
			if (!_isUserOnline && (message == "1" || message == "2" || message == "0"))
				_server.sendMessage("1 - Operation is correct");
			else if (_isUserOnline && message == "3" || message == "4" || message == "5" || message == "6" || message == "7")
				_server.sendMessage("1 - Operation is correct");
			else 
				_server.sendMessage("2 - Operation is incorrect");
			
			switch (std::stoi(message))
			{
			case 1:
				std::cout << "Enter LOGIN menu" << std::endl;
				loginOk = false;
				passwordOk = false;
				while (!loginOk) {
					login = _server.readMessage();
					if (_dataBase.checkUserByLogin(login)) {
						loginOk = true;
						std::cout << "User's login: " << login << std::endl;
						_server.sendMessage("1");
					}
					else {
						_server.sendMessage("0");
					}
					while (!passwordOk) {
						password = _server.readMessage();
						if (_dataBase.checkUserByLoginAndPassword(login, password)) {
							_currentUser = login;
							_isUserOnline = true;
							passwordOk = true;
							std::cout << "User's password is correct!" << std::endl;
							_server.sendMessage("1");
						}
						else
							_server.sendMessage("0");
					}
				}
				break;
			case 2:
				std::cout << "Enter SIGNUP menu" << std::endl;
				name = _server.readMessage();
				std::cout << "User's name: " << name << std::endl;
				while (!loginOk) {
					message = _server.readMessage();
					if (_dataBase.checkUserByLogin(message))
						_server.sendMessage("1");
					else {
						login = message;
						std::cout << "User's login: " << login << std::endl;
						loginOk = true;
						_server.sendMessage("0");
					}
				}
				password = _server.readMessage();
				std::cout << "User's password: " << password << std::endl;
				_dataBase.addUser(name, login, password);
				_server.sendMessage("Adding a user was successful!");
				_currentUser = login;
				_isUserOnline = true;
				break;
			case 3:
				std::cout << "Enter SHOW ALL USERS IN CHAT menu" << std::endl;
				users = _dataBase.allUsersInChat();
				_server.sendMessage(std::to_string(users.size()));
				_server.readMessage();
				for (int i = 0; i < users.size(); i++) {
					_server.sendMessage(users[i]);
					_server.readMessage();
				}
				break;
			case 4:
				std::cout << "Enter SHOW ALL MESSAGES menu" << std::endl;
				message = _server.readMessage();
				switch (std::stoi(message))
				{
				case 1:
					userMessages.clear();
					userMessages = _dataBase.showMessagesToAll();
					_server.sendMessage(std::to_string(userMessages.size()));
					if (userMessages.size() == 0)
						std::cout << "It's no messages to all in database!" << std::endl;
					else {
						std::cout << "Messages to all: " << userMessages.size() << std::endl;
						_server.readMessage();
						for (int i = 0; i < userMessages.size(); i++) {
							_server.sendMessage(userMessages[i]);
							_server.readMessage();
						}
					}
					break;
				case 2:
					userMessages.clear();
					userMessages = _dataBase.showPrivateMessages(_currentUser);
					_server.sendMessage(std::to_string(userMessages.size()));
					if (userMessages.size() == 0)
						std::cout << "It's no messages to user with login" << _currentUser << "in database!" << std::endl;
					else {
						std::cout << "Messages to user with login" << _currentUser << ": " << userMessages.size() << std::endl;
						_server.readMessage();
						for (int i = 0; i < userMessages.size(); i++) {
							_server.sendMessage(userMessages[i]);
							_server.readMessage();
						}
					}
					break;
				default:
					break;
				}
				break;
			case 5:
				std::cout << "Enter ADD MESSAGE TO CHAT menu" << std::endl;
				loginOk = false;
				while (!loginOk) {
					to = _server.readMessage();
					if (!(to == "all")) {
						if (_dataBase.checkUserByLogin(to)) {
							loginOk = true;
							std::cout << "Message to: " << to << std::endl;
							_server.sendMessage("1");
							text = _server.readMessage();
							_dataBase.addPrivateMessage(_currentUser, to, text);
						}
						else
							_server.sendMessage("0");
					}
					else {
						loginOk = true;
						text = _server.readMessage();
						_dataBase.addMessageToAll(_currentUser, text);
					}
				}
				break;
			case 6:
				std::cout << "Enter CHANGE PASSWORD menu" << std::endl;
				newPassword = _server.readMessage();
				if (_dataBase.changePassword(_currentUser, newPassword)) {
					_server.sendMessage("1");
				}
				else {
					_server.sendMessage("0");
				}
				break;
			case 7:
				std::cout << "Enter SIGN OUT menu" << std::endl;
				std::cout << "User offline" << std::endl;
				_isUserOnline = false;
				break;
			case 0:
				std::cout << "Enter EXIT menu" << std::endl;
				std::cout << "Thank you for use Console Chat! Good Bye!" << std::endl;
				_connection = false;
				_server.closeServer();
				break;
			default:
				break;
			}
		}
	}

private:
	std::string _currentUser;	/** имя текущего пользователя, находящегося в чате */
	bool _isUserOnline;			/** признак наличия пользователя в чате */
	bool _connection;

	Server _server;				/** объект для работы с сетью */
	std::string _ip;		/** IP сервера */
	uint16_t _port;			/** номер порта сервера */

	Database _dataBase;			/** объект БД */
	std::string _dbHost;		/** адрес БД */
	std::string _dbUser;		/** пользователь БД */
	std::string _dbPassword;	/** пароль пользователя БД */
	std::string _dbName;		/** имя БД */
};