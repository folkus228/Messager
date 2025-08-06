#pragma once

#include "process_tools.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using namespace std;

string getMessageFromServer(tcp::socket& socket, string& message);

bool login(string& name, string& tag, string& password, string& phone_number, tcp::socket& socket)
{
    string error;
    while (true)
    {
        int choise = 0;
        cout << "вход - 1" << '\n'
            << "регистрация - 2" << '\n'
            << "~> ";
        cin >> choise;
        if (std::cin.fail()) {
            // Если ввод не является числом
            std::cin.clear(); // очищаем состояние потока
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // пропускаем неправильный ввод
            std::cout << "\033[91m" << "Ошибка: введено не число. Попробуйте снова." << "\033[0m" << '\n';
            continue;
        }

        if (choise == 1)
        {
            cout << "tag - ";
            cin >> tag;
            cout << "пароль - ";
            cin >> password;

            string request = "0 " + tag + ' ' + password;

            
            stringstream ss(getMessageFromServer(socket, request));
            ss >> error;
            if (error == "Error")
            {
                cout << "\033[91m" << "Ошибка: tag или пароль неверный. Попробуйте снова." << "\033[0m" << endl;
                continue;
            }
            
            ss >> name >> phone_number >> phone_number;
            if (phone_number == password)
                phone_number = "";

            break;
        }
        else if (choise == 2)
        {
            cout << "tag - ";
            cin >> tag;
            cout << "имя - ";
            cin >> name;
            cout << "пароль - ";
            cin >> password;
            cout << "номер телефона (введи \"null\" если нет номера) - ";
            cin >> phone_number;

            if (phone_number == "null") { phone_number = ""; }

            string request = "3 " + tag + ' ' + name + ' ' + password + ' ' + phone_number;

            error = getMessageFromServer(socket, request);
            if (error == "Error")
            {
                cout << "Хмм, наверное такой tag занят, заново" << endl;
                continue;
            }
            break;
        }
        else
        {
            cout << "\033[91m" << "Такого нет" << "\033[0m" << endl;
            continue;
        }
    }

    cout << "\033[92m" << "Вы зашли как - " << name << "\033[0m" << endl;

    return true;
}

int getChat(string& tag, tcp::socket& socket)
{
    // создать или выбрать
    // разом
    // и просто вернуть id чата

    cout << endl << "Выбор чата:" << endl;

    string request = "1 " + tag;
    string chats = getMessageFromServer(socket, request);

    string buffer = "";
    for (int i = 0; i < chats.size(); i++)
    {
        if (chats[i] == '\0') break;
        buffer += chats[i];
    }

    vector<string> vec_chats;
    splitLines(vec_chats, chats);

    int id;
    string owner;
    string name;
    for (size_t i = 0; i < vec_chats.size(); i++)
    {
        processLineChat(vec_chats[i], id, owner, name);
        cout << i << " - ";
        for (size_t i = 0; i < name.size(); i++) // это только для вывода name
        {                                        // не понимаю почему он выводит огромнуб строку когда в debug выглядит так:
            if (name[i] != '\0')                 // test\0 что-то что-то что-то 
                cout << name[i];                 // но при выводе игнорируется \0 
            else
                break;
        }
        cout << endl;
    }
    cout << vec_chats.size() << " - log out" << endl;
    while (true)
    {
        int choice;
        cin >> choice;
        if (std::cin.fail()) {
            // Если ввод не является числом
            std::cin.clear(); // очищаем состояние потока
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // пропускаем неправильный ввод
            std::cout << "\033[91m" << "Ошибка: введено не число. Попробуйте снова." << "\033[0m" << '\n';
            continue;
        }
        if (choice == vec_chats.size())
        {
            return -1;
        }
        else if (0 <= choice && choice < vec_chats.size())
        {
            cout << "\033[92m" << "Выбран чат - ";
            for (size_t i = 0; i < name.size(); i++) // это только для вывода name
            {                                        // не понимаю почему он выводит огромнуб строку когда в debug выглядит так:
                if (name[i] != '\0')                 // test\0 что-то что-то что-то 
                    cout << name[i];                 // но при выводе игнорируется \0 
                else
                    break;
            }
            cout << "\033[0m" << endl;

            processLineChat(vec_chats[choice], id, owner, name);
            return id;
        }
        else
        {
            std::cout << "\033[91m" << "Ошибка: введено несуществующее чило. Попробуйте снова." << "\033[0m" << '\n';
        }
    }

    return -1;
}

void printMessages(const int& chat_id, const int count, tcp::socket& socket)
{
    // сразу вывод сообщений 
    // либо выход 
    // либо отправить сообщение и тогда перезапросить сообщения

    string request = "2 " + to_string(chat_id) + " 0 " + to_string(count);
    string messages = getMessageFromServer(socket, request);

    if (messages[0] == '\0') return; // ничего нет

    vector<string> vec_messages;
    splitLines(vec_messages, messages);

    int id;
    string tag;
    string date;
    string content;
    string name;
    for (size_t i = 0; i < vec_messages.size(); i++)
    {
        processLineMessage(vec_messages[i], id, tag, date, content);
        
        request = "10 " + tag;
        name = getMessageFromServer(socket, request);

        for (int i = 0; i < name.size() && name[i] != '\0'; i++) // если просто выводить name то выведены будут и символы после \0
        {
            cout << name[i];
        }

        cout << ": " << content << endl;
    }
}

void addInWhitelist(const int& chat_id, const string& tag, tcp::socket& socket)
{
    string request = "6 " + tag + ' ' + to_string(chat_id);
    if (getMessageFromServer(socket, request) == "Y")
    {
        cout << "\033[92m" << "Успешно добавлен" << "\033[0m" << endl;
    }
    else
    {
        cout << "\033[91m" << "Не удалось добавить" << "\033[0m" << endl;
    }
}

int controlMessage(const int& chat_id, const int count, const string& tag, tcp::socket& socket)
{    
    while (true)
    {     
        printMessages(chat_id, count, socket);

        string message;
        while (true)
        {
            cout << "|exit - для выхода|\n|/add \'tag\'        |\n ~> ";
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            getline(cin, message);

            if (message == "exit") return -1;

            stringstream ss(message);
            string buffer;
            ss >> buffer;
            if (buffer == "/add")
            {
                ss >> buffer;
                addInWhitelist(chat_id, buffer, socket);
                continue;
            }

            break;
        }

        string request = "5 " + tag + ' ' + to_string(chat_id) + ' ' + message;
        if (getMessageFromServer(socket, request) == "N")
        {
            cout << "\033[91m" << "Сообщение не удалось отправить" << "\033[0m" << endl;
        }

        cout << endl << endl;
    }
}