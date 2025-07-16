#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>

using boost::asio::ip::tcp;

class TcpClient
{
public:
    TcpClient(const std::string& host, const std::string& port, std::size_t thread_pool_size)
        : io_context_(), socket_(io_context_), resolver_(io_context_),
        thread_pool_size_(thread_pool_size)
    {
        connect(host, port);
    }

    void run()
    {
        // Создаем пул потоков для обработки операций ввода-вывода
        std::vector<std::thread> threads;
        for (std::size_t i = 0; i < thread_pool_size_; ++i)
        {
            threads.emplace_back([this]() { io_context_.run(); });
        }

        // Ждем завершения всех потоков
        for (std::size_t i = 0; i < thread_pool_size_; ++i)
        {
            threads[i].join();
        }
    }

    void send(const std::string& message)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            bool write_in_progress = !write_msgs_.empty();
            write_msgs_.push_back(message);
            if (!write_in_progress)
            {
                boost::asio::post(io_context_, [this]() { write(); });
            }
        }
    }

    void close()
    {
        boost::asio::post(io_context_, [this]() { socket_.close(); });
    }

private:
    void connect(const std::string& host, const std::string& port)
    {
        resolver_.async_resolve(host, port,
            [this](const boost::system::error_code& ec, tcp::resolver::results_type endpoints)
            {
                if (!ec)
                {
                    boost::asio::async_connect(socket_, endpoints,
                        [this](const boost::system::error_code& ec, const tcp::endpoint&)
                        {
                            if (!ec)
                            {
                                std::cout << "Connected to server" << std::endl;
                                read();
                            }
                            else
                            {
                                std::cerr << "Connect failed: " << ec.message() << std::endl;
                            }
                        });
                }
                else
                {
                    std::cerr << "Resolve failed: " << ec.message() << std::endl;
                }
            });
    }

    void read()
    {
        socket_.async_read_some(boost::asio::buffer(read_buffer_),
            [this](const boost::system::error_code& ec, std::size_t length)
            {
                if (!ec)
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    std::cout << "Received: " << std::string(read_buffer_.data(), length)
                        << " in thread " << std::this_thread::get_id() << std::endl;
                    read();
                }
                else
                {
                    std::cerr << "Read failed: " << ec.message() << std::endl;
                    socket_.close();
                }
            });
    }

    void write()
    {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front()),
            [this](const boost::system::error_code& ec, std::size_t /*length*/)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!ec)
                {
                    write_msgs_.pop_front();
                    if (!write_msgs_.empty())
                    {
                        write();
                    }
                }
                else
                {
                    std::cerr << "Write failed: " << ec.message() << std::endl;
                    socket_.close();
                }
            });
    }

    boost::asio::io_context io_context_;
    tcp::socket socket_;
    tcp::resolver resolver_;
    std::array<char, 1024> read_buffer_;
    std::deque<std::string> write_msgs_;
    std::mutex mutex_;
    std::size_t thread_pool_size_;
};

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru");

#ifdef _DEBUG
    std::cout << "multi core is work" << std::endl;
    std::cout << "args count: " << argc << std::endl << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << i << ": " << argv[i] << std::endl;
    }
    std::cout << std::endl;
#endif

    try
    {
        if (argc < 4)
        {
            std::cerr << "Usage: client <host> <port> <thread_pool_size>\n";
            return 1;
        }

        {
            int a;
            std::cout << "start?" << std::endl;
            std::cin >> a;
        }

        TcpClient client(argv[1], argv[2], std::atoi(argv[3]));

        std::thread client_thread([&client]() { client.run(); });

        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line == "quit")
            {
                client.close();
                break;
            }
            client.send(line);
        }

        client_thread.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}