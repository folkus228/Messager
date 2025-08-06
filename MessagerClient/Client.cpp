#include <boost/asio.hpp>
#include "process_client.h"
#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using namespace std;

//string getMessageFromServer(tcp::socket& socket, string& message);


string getMessageFromServer(tcp::socket& socket, string& message)
{
    if (message.empty()) return ""; // выход при пустой строке

    // Отправляем данные
    boost::asio::write(socket, boost::asio::buffer(message + "\n"));

    // Читаем ответ от сервера
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\0"); // чета здесь мб сокет неправильно передал

    std::string reply((std::istreambuf_iterator<char>(&response)),
        std::istreambuf_iterator<char>());

    return reply;

}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru");

    try {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
            return 1;
        }

        string host = argv[1];
        string port = argv[2];

        boost::asio::io_context io_context;

        // Резолвим хост и порт
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);

        tcp::socket socket(io_context);

        // Подключаемся к серверу
        boost::asio::connect(socket, endpoints);
        cout << "Connected to server " << host << ":" << port << endl;


        string name;
        string tag;
        string password;
        string phone_number;
        while (true) {
            if (!login(name, tag, password, phone_number, socket))
                continue;

            int chat_id = getChat(tag, socket);

            controlMessage(chat_id, 5, tag, socket);
            
            //string message;
            //getline(cin, message);
            //cout << getMessageFromServer(socket, message) << endl;

        }
    }
    catch (std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
