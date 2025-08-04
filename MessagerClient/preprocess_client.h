#pragma once

#include "chat_tools.h"
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

    string request = "1 " + tag;
    string chats = getMessageFromServer(socket, request);

    vector<string> vec_chats;
    splitLines(vec_chats, chats);

    int id;
    string owner;
    string name;
    for (size_t i = 0; i < vec_chats.size(); i++)
    {
        processLine(vec_chats[i], id, owner, name);
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
            return choice;
        }
        else
        {
            std::cout << "\033[91m" << "Ошибка: введено несуществующее чило. Попробуйте снова." << "\033[0m" << '\n';
        }
    }

    return -1;
}



