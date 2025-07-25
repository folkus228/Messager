#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <mysql.h>
using namespace std;

class Messagerdb
{
public:

	Messagerdb(const char* host, const char* user, const char* password,
		const char* db, unsigned int&& port, const char* unix_socket, unsigned long&& clientflag)
		: answer(new string)
	{
		// Получаем дескриптор соединения
		mysql_init(&mysql);
		if (&mysql == nullptr) {
			// Если дескриптор не получен — выводим сообщение об ошибке
			cerr << "Error: can't create MySQL-descriptor" << endl;
		}

		// Подключаемся к серверу
		if (!mysql_real_connect(&mysql, host, user, password, db, port, unix_socket, clientflag)) {
			// Если нет возможности установить соединение с БД выводим сообщение об ошибке
			cerr << "Error: can't connect to database " << mysql_error(&mysql) << endl;
		}
		else {
			// Если соединение успешно установлено выводим фразу — "Success!"
			cout << "Success!" << endl;
		}

		mysql_set_character_set(&mysql, "utf8");
		//Смотрим изменилась ли кодировка на нужную, по умолчанию идёт latin1
		cout << "connection characterset: " << mysql_character_set_name(&mysql) << endl;
	}
	~Messagerdb()
	{
		// Закрываем соединение с сервером базы данных
		mysql_close(&mysql);
		delete answer;
	}

	const string* getUserInfo(const string& tag, const string& password)
	{
		string query = "select * from clients where clients.tag = \'" + tag + "\' and clients.password = \'" + password + "\'";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		//Выводим все что есть в базе через цикл
		if (res = mysql_store_result(&mysql)) {
			row = mysql_fetch_row(res);
			
			if (row == nullptr)
			{
				cout << "\033[91m" << "Ошибка пользователя: пользователя несуществует" << "\033[0m" << endl;
				*answer = "Error";
				return answer;
			}

			*answer = string(row[0]) + ' ' + string(row[1]) + ' ' + string(row[2]);
			if (row[3] != NULL)
				*answer += ' ' + string(row[3]);

			cout << "\033[94m" << "from db: " << *answer << "\033[0m" << endl;			
		}
		else
		{
			cout << "\033[91m" << "Ошибка MySql номер " << mysql_error(&mysql) << "\033[0m";
			*answer = "Error";
		}
			
		return answer;
	}
	const string* getChats(const string& tag)
	{
		string query = "SELECT chats.name, chats.owner_tag FROM chats LEFT JOIN whitelist ON whitelist.chat_id = chats.id where client_tag = \'" + tag + "\'";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		//Выводим все что есть в базе через цикл
		if (res = mysql_store_result(&mysql)) {
			
			while (row = mysql_fetch_row(res)) {
				//if (row == nullptr)
				//{
				//	cout << "\033[91m" << "Ошибка пользователя: пользователя несуществует" << "\033[0m" << endl;
				//	*answer = "Error";
				//	return *answer;
				//}
				*answer += string(row[0]) + ' ' + string(row[1]) + '\n';
			}
			if (!answer->empty())
				answer->pop_back();
			cout << "\033[94m" << "from db:\n" << *answer << "\033[0m" << endl;
		}
		else 
		{
			cout << "\033[91m" << "Ошибка MySql номер " << mysql_error(&mysql) << "\033[0m";
			*answer = "Error";
		}
		return answer;
	}
	const string* getMessages(const int& chat_id, const int& count, const int& message_id)
	{
		string query = "select id, client_tag, date, content from messages where chat_id = " + to_string(chat_id) 
			+ " and id > " + to_string(message_id) + " order by date desc limit " + to_string(count);
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		//Выводим все что есть в базе через цикл
		if (res = mysql_store_result(&mysql)) {
			while (row = mysql_fetch_row(res)) {
				*answer += string(row[0]) + ' ' + string(row[1]) + ' ' + 
					string(row[2]) + ' ' + string(row[3]) + '\n';
			}
			if (!answer->empty())
				answer->pop_back();
			cout << "\033[94m" << "from db:\n" << *answer << "\033[0m" << endl;
		}
		else
			cout << "Ошибка MySql номер " << mysql_error(&mysql);
		return answer;
	}

	bool addUser(const string& tag, const string& name, const string& password, const string& phone_number) 
	{
		string query;
		if (phone_number.empty())
		{
			query = "insert into clients(tag, name, password) values(\'" 
				+ tag + "\', \'" + name + "\', \'" + password + "\')";
		}
		else
		{
			query = "insert into clients(tag, name, password, phone_number) values(\'"
				+ tag + "\', \'" + name + "\', \'" + password + "\', \'" + phone_number + "\')";
		}
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице
		
		const char* error = mysql_error(&mysql);
		if (*error == '\0' || error == nullptr)
		{
			cout << "\033[92m" << "user added!" << "\033[0m" << endl;
			return true;
		}
		else
		{
			cout << "Ошибка MySql номер: " << error << endl;
			return false;
		}
	}
	bool addChat(const string& name, const string& owner_tag) 
	{
		string query = "insert into chats(name, owner_tag) values(\'"
			+ name + "\', \'" + owner_tag + "\')";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		const char* error = mysql_error(&mysql);
		if (*error == '\0' || error == nullptr)
		{
			cout << "\033[92m" << "chat added!" << "\033[0m" << endl;
			return true;
		}
		else
		{
			cout << "Ошибка MySql номер: " << error << endl;
			return false;
		}
	}
	bool addMessage(const string& client_tag, const int& chat_id, const string& content)
	{
		string query = "insert into messages(client_tag, chat_id, content) values(\'"
			+ client_tag + "\', " + to_string(chat_id) + ", \'" + content + "\')";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		const char* error = mysql_error(&mysql);
		if (*error == '\0' || error == nullptr)
		{
			cout << "\033[92m" << "message sended!" << "\033[0m" << endl;
			return true;
		}
		else
		{
			cout << "Ошибка MySql номер: " << error << endl;
			return false;
		}
	}
	bool add_in_whitelist(const string& client_tag, const int& chat_id)
	{
		string query = "insert into whitelist(client_tag, chat_id) values (\'" + client_tag + "\', " + to_string(chat_id) + ")";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		const char* error = mysql_error(&mysql);
		if (*error == '\0' || error == nullptr)
		{
			cout << "\033[92m" << "add in whitelist!" << "\033[0m" << endl;
			return true;
		}
		else
		{
			cout << "Ошибка MySql номер: " << error << endl;
			return false;
		}
	}
	bool add_administrator(const string& client_tag, const int& chat_id)
	{
		string query = "insert into administratorslist(client_tag, chat_id) values (\'" + client_tag + "\', " + to_string(chat_id) + ")";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		const char* error = mysql_error(&mysql);
		if (*error == '\0' || error == nullptr)
		{
			cout << "\033[92m" << "add administrator!" << "\033[0m" << endl;
			return true;
		}
		else
		{
			cout << "Ошибка MySql номер: " << error << endl;
			return false;
		}
	}
	bool del_from_whitelist(const string& client_tag, const int& chat_id)
	{
		string query = " delete from whitelist where client_tag = \'" + client_tag + "\' and chat_id = " + to_string(chat_id) + ")";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		const char* error = mysql_error(&mysql);
		if (*error == '\0' || error == nullptr)
		{
			cout << "\033[92m" << "del from whitelist!" << "\033[0m" << endl;
			return true;
		}
		else
		{
			cout << "Ошибка MySql номер: " << error << endl;
			return false;
		}
	}
	bool del_administrator(const string& client_tag, const int& chat_id)
	{
		string query = " delete from administratorslist where client_tag = \'" + client_tag + "\' and chat_id = " + to_string(chat_id) + ")";
		mysql_query(&mysql, query.c_str()); //Делаем запрос к таблице

		const char* error = mysql_error(&mysql);
		if (*error == '\0' || error == nullptr)
		{
			cout << "\033[92m" << "del administrator!" << "\033[0m" << endl;
			return true;
		}
		else
		{
			cout << "Ошибка MySql номер: " << error << endl;
			return false;
		}
	}

private:

	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;

	string* answer;

};

