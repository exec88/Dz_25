#pragma once

#include <mysql.h>
#include <fstream>
#include <string>
#include <vector>

/**
* Класс для работы с базой данных чата.
*/
class Database {
public:
	Database() {};

	~Database() 
	{	// Закрываем соединение с базой данных
		mysql_close(_mysql);
	}

	// Подключение к БД.
	void connect(const std::string& host, const std::string& user, const std::string& password, const std::string& name) {
		
		_dbHost = host;
		_dbUser = user;
		_dbPassword = password;
		_dbName = name;

		_mysql = mysql_init(NULL);

		if (_mysql == NULL) {
			// Если дескриптор не получен — выводим сообщение об ошибке
			std::cout << "Error: can't create MySQL-descriptor" << std::endl;
			return;
		}
		// Подключаемся к серверу
		if (!mysql_real_connect(_mysql, _dbHost.c_str(), _dbUser.c_str(), _dbPassword.c_str(), NULL, 0, NULL, 0)) {
			// Если нет возможности установить соединение с БД выводим сообщение об ошибке
			std::cout << "Error: can't connect to database " << mysql_error(_mysql) << std::endl;
			return;
		}
		else {
			// Если соединение успешно установлено выводим фразу — "Success!"
			std::cout << "Connect to database - success!" << std::endl;
		}

		// Пытаемся создать базу данных. Если база данных существует, она не будет перезаписана.
		std::string query = "create database if not exists " + _dbName;
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			std::cout << "Data base is created or finded!" << std::endl;
		}

		// Подключаемся к нашей базе данных
		_mysql = mysql_init(NULL);
		if (!mysql_real_connect(_mysql, _dbHost.c_str(), _dbUser.c_str(), _dbPassword.c_str(), _dbName.c_str(), 0, NULL, 0)) {
			// Если нет возможности установить соединение с БД выводим сообщение об ошибке
			std::cout << "Error: can't connect to database " << _dbName << "  " << mysql_error(_mysql) << std::endl;
			return;
		}
		else {
			// Если соединение успешно установлено выводим фразу — "Success!"
			std::cout << "Connect to database " << _dbName << " - success!" << std::endl;
		}

		createTables(); // создаем таблицы если еще не созданы

		mysql_set_character_set(_mysql, "utf8mb4");
	}

	bool checkUserByLogin(std::string& login) {
		std::string query = "select id from Users where login='" + login + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			if(row = mysql_fetch_row(res))
				return true;
			else 
				return false;
		}
	}

	void addUser(const std::string& name, const std::string& login, const std::string& password) {
		std::string query = "insert into Users(id, name, login, password) values(default, '" +
			name + "', '" + login + "', '" + password + "')";
		int res = mysql_query(_mysql, query.c_str());
		if (res == 0) {
			std::cout << "User with login " << login << " successfully added" << std::endl;
		}
		else {
			std::cout << "Error adding a user with login " << login << " - " << mysql_error(_mysql) << std::endl;
		}
	}

	bool checkUserByLoginAndPassword(const std::string& login, const std::string& password) {
		std::string query = "select password from Users where login='" + login + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				if (row[0] == password)
					return true;
				else
					return false;
			}
		}
	}

	std::vector<std::string> allUsersInChat() {
		std::vector<std::string> users{};
		std::string query = "select login from Users";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				users.push_back(row[0]);
			}
		}
		return users;
	}

	std::string getUserId(const std::string& login) {
		std::string query = "select id from Users where login='" + login + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			row = mysql_fetch_row(res);
			return row[0];
		}
	}

	std::string getUserLoginById(const std::string& id) {
		std::string query = "select login from Users where id='" + id + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			row = mysql_fetch_row(res);
			return row[0];
		}
	}

	void addPrivateMessage(const std::string& sender, const std::string& reciever, const std::string& text) {
		std::string senderId = getUserId(sender);
		std::string recieverId = getUserId(reciever);
		std::string query = "insert into MessagesPrivate(id, senderId, recieverId, text) values(default, '" +
			senderId + "', '" + recieverId + "', '" + text + "')";
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			std::cout << "The message was successfully added" << std::endl;
		}
		else {
			std::cout << "Error adding a message - " << mysql_error(_mysql) << std::endl;
		}
	}

	void addMessageToAll(const std::string& sender, const std::string& text) {
		std::string senderId = getUserId(sender);
		std::string query = "insert into MessagesToAll(id, senderId, text) values(default, '" +
			senderId + "', '" + text + "')";
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			std::cout << "The message was successfully added" << std::endl;
		}
		else {
			std::cout << "Error adding a message - " << mysql_error(_mysql) << std::endl;
		}
	}

	std::vector<std::string> showPrivateMessages(const std::string& reciever) {
		std::vector<std::string> messagesPrivate;
		std::string message{};
		std::string query = "select * from MessagesPrivate";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				if(getUserLoginById(row[2]) == reciever) {
					message = "From: " + getUserLoginById(row[1]) + "\tTo: Me" + "\nText: " + row[3];
					messagesPrivate.push_back(message);
				}
			}
		}
		return messagesPrivate;
	}

	std::vector<std::string> showMessagesToAll() {
		std::vector<std::string> messagesToAll{};
		std::string message{};
		std::string query = "select * from MessagesToAll";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				message = "From: " + getUserLoginById(row[1]) + "\tTo: All" + "\nText: " + row[2];
				messagesToAll.push_back(message);
			}
		}
		return messagesToAll;
	}

	bool changePassword(const std::string& login, const std::string& newPassword) {
		std::string id = getUserId(login);
		std::string query = "update Users set password=" + newPassword + " where id=" + id + "";
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			std::cout << "The password was successfully changed" << std::endl;
			return true;
		}
		else {
			std::cout << "Error change a password - " << mysql_error(_mysql) << std::endl;
			return false;
		}
	}

private:
	MYSQL* _mysql;				// дескриптор соединения с базой данных.
	std::string _dbHost;		// имя хоста базы данных
	std::string _dbUser;		// имя пользователя базы данных
	std::string _dbPassword;	// пароль пользователя
	std::string _dbName;		// имя базы данных

	void createTables() {
		int result{};
		// создаем таблицу пользователей
		result = mysql_query(_mysql, "create table if not exists Users"
			"(id int auto_increment primary key, " //
			"name varchar(255), "
			"login varchar(255) not null, "
			"password varchar(255) not null)");
		if (result == 0) {
			std::cout << "Table Users is created or finded!" << std::endl;
		}
		else {
			std::cout << "Table Users is not created or not finded!" << std::endl;
		}

		// создаем таблицу приватных сообщений
		result = mysql_query(_mysql, "create table if not exists MessagesPrivate"
			"(id int auto_increment primary key,"
			"senderId int references Users(id), "
			"recieverId int references Users(id), "
			"text varchar(1024))");
		if (result == 0) {
			std::cout << "Table MessagesPrivate is created or finded!" << std::endl;
		}
		else {
			std::cout << "Table MessagesPrivate is not created or not finded!" << std::endl;
		}

		// создаем таблицу сообщений всем
		result = mysql_query(_mysql, "create table if not exists MessagesToAll"
			"(id int auto_increment primary key,"
			"senderId int references Users(id), "
			"text varchar(1024))");
		if (result == 0) {
			std::cout << "Table MessagesToAll is created or finded!" << std::endl;
		}
		else {
			std::cout << "Table MessagesToAll is not created or not finded!" << std::endl;
		}
	}
};

