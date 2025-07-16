#include <iostream>
#include <deque>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TcpClient
{
public:
    TcpClient(boost::asio::io_context& io_context,
        const std::string& host, const std::string& port)
        : io_context_(io_context), socket_(io_context), resolver_(io_context)
    {
        connect(host, port);
    }

    void send(const std::string& message)
    {
        boost::asio::post(io_context_,
            [this, message]()
            {
                bool write_in_progress = !write_msgs_.empty();
                write_msgs_.push_back(message);
                if (!write_in_progress)
                {
                    write();
                }
            });
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
                    std::cout << "\033[92m" << "Received: " << "\033[0m" << std::string(read_buffer_.data(), length) << std::endl;
                    read();
                }
                else
                {
                    std::cerr << "\033[91m" << "Read failed: " << "\033[0m" << ec.message() << std::endl;
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

    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    tcp::resolver resolver_;
    std::array<char, 1024> read_buffer_;
    std::deque<std::string> write_msgs_;
};

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
#endif

    try
    {
        if (argc < 3)
        {
            std::cerr << "Usage: client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        TcpClient client(io_context, argv[1], argv[2]);

        std::thread t([&io_context]() { io_context.run(); });

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

        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}