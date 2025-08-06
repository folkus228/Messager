#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include "Server_async.h"
#include "Messagerdb.h"

//enum class RequestType : short
//{
//    getUserInfo = 0,
//    getChats = 1,
//    getMessages = 2,
//    addUser = 3,
//    addChat = 4,
//    addMessage = 5,
//    add_in_whitelist = 6,
//    add_administrator = 7,
//    del_from_whitelist = 8,
//    del_administrator = 9,
//    get_user_name = 10,
//};

void TestColors() {
    std::cout << "Тест цветов" << std::endl;
    for (int i = 0; i < 100; i++)
    {
        std::cout << "\033[" << i << "m" << "Номер: " << i << std::endl;
    }
}

// размер буфера 1024 всегда 
// возвратить true если надо отправить что-то взамен инче false
bool request_func(char* data, int length, boost::asio::ip::tcp::socket& socket)
{
    static Messagerdb APIdb("localhost", "root", "root", "Messager", NULL, NULL, 0); // тут поменять

    static string tag;
    static string name;
    static string password;
    static string phone_number;
    static string content;
    static int chat_id;
    static int message_id;
    static int count;

    stringstream request(std::string(data, length));

    short requestType;
    request >> requestType;
    switch (requestType)
    {
    case 0:    
        request >> tag;
        request >> password;
        cout << "\033[95m" << "tag: " << tag << '\n' << "password: " << password << endl;
        strcpy(data, APIdb.getUserInfo(tag, password)->c_str());
        break;   
    case 1:
        request >> tag;
        strcpy(data, APIdb.getChats(tag)->c_str());
        break;
    case 2:
        request >> chat_id;
        request >> message_id;
        request >> count; // с какого начинать
        strcpy(data, APIdb.getMessages(chat_id, message_id, count)->c_str());
        break;
    case 3:
        request >> tag;
        request >> name;
        request >> password;
        request >> phone_number;
        strcpy(data, APIdb.addUser(tag, name, password, phone_number) ? "Y" : "N");
        break;
    case 4:
        request >> name;
        request >> tag;
        strcpy(data, APIdb.addChat(name, tag) ? "Y" : "N");
        break;
    case 5:
        request >> tag;
        request >> chat_id;
        request >> content; // тут допилить на несколько слов
        strcpy(data, APIdb.addMessage(tag, chat_id, content) ? "Y" : "N");
        break;
    case 6:
        request >> tag;
        request >> chat_id;
        strcpy(data, APIdb.add_in_whitelist(tag, chat_id) ? "Y" : "N");
        break;
    case 7:
        request >> tag;
        request >> chat_id;
        strcpy(data, APIdb.add_administrator(tag, chat_id) ? "Y" : "N");
        break;
    case 8:
        request >> tag;
        request >> chat_id;
        strcpy(data, APIdb.del_from_whitelist(tag, chat_id) ? "Y" : "N");
        break;
    case 9:
        request >> tag;
        request >> chat_id;
        strcpy(data, APIdb.del_administrator(tag, chat_id) ? "Y" : "N");
        break;
    case 10:
        request >> tag;
        strcpy(data, APIdb.getUserName(tag)->c_str());
        break;
    default:
        break;
    }

    std::cout << "length: " << length << std::endl;
    
    std::cout << std::string(data, length) << std::endl;
    return true;
}


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru");

#ifdef _DEBUG
    std::cout << "args count: " << argc << std::endl << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << i << ": " << argv[i] << std::endl;
    }
    std::cout << std::endl;

    TestColors();
#endif

    try
    {
        if (argc < 3)
        {
            std::cerr << "Usage: server <port> <thread_pool_size>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        TcpServer server(io_context, std::atoi(argv[1]), std::atoi(argv[2]), &request_func);

        std::cout << "Server started on port " << argv[1]
            << " with " << argv[2] << " threads" << std::endl;
        server.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "\033[91m" << "Exception: " << e.what() << "\n";
    }

    return 0;
}