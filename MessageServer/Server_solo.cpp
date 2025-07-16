#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
    TcpSession(tcp::socket socket) : socket_(std::move(socket)) {}

    void start()
    {
        read();
    }

private:
    void read()
    {
        std::shared_ptr<TcpSession> self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    std::cout << "Received: " << std::string(data_, length) << std::endl;
                    write(length);
                }
            });
    }

    void write(std::size_t length)
    {
        std::shared_ptr<TcpSession> self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    read();
                }
            });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class TcpServer
{
public:
    TcpServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        accept();
    }

private:
    void accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::cout << "New connection from: " << socket.remote_endpoint() << std::endl;
                    std::make_shared<TcpSession>(std::move(socket))->start();
                }

                accept();
            });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{

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
        if (argc < 2)
        {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        TcpServer server(io_context, std::atoi(argv[1]));

        std::cout << "Server started on port " << argv[1] << std::endl;
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}