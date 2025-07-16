#include "Server_async.h"
#include "dbAPI.h"


void TestColors() {
    std::cout << "Тест цветов" << std::endl;
    for (int i = 0; i < 100; i++)
    {
        std::cout << "\033[" << i << "m" << "Номер: " << i << std::endl;
    }
}

bool request_func(char* data, int length, boost::asio::ip::tcp::socket socket)
{
    std::cout << "request func" << std::endl;
    return true;
}


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru");

#ifdef _DEBUG
    std::cout << " multicore is work" << std::endl;
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