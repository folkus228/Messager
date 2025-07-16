#include "Server_async.h"

using boost::asio::ip::tcp;

TcpSession::TcpSession(tcp::socket socket,
    bool(**inRequest)(char* data, int length, tcp::socket socket)) 
    : socket_(std::move(socket)), inRequest_(inRequest) {}

void TcpSession::start()
{
    read();
}

void TcpSession::read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        /*[this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                //std::cout << "\033[92m" << "Received: "
                    //<< "\033[0m" << "\033[52m" << std::string(data_, length)
                    //<< "\033[0m" << " from " << "\033[92m" << socket_.remote_endpoint()
                    //<< "\033[0m" << " in thread " << std::this_thread::get_id() << std::endl;
                write(length);
            }
        }*/
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                //if (true == (*inRequest_)(data_, max_length, std::move(socket_)))
                {
                }
                write(length);
            }
        }
        );
}

void TcpSession::write(std::size_t length)
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                read();
            }
        });
}

TcpServer::TcpServer(boost::asio::io_context& io_context, short port, std::size_t thread_pool_size,
    bool(*inRequest)(char* data, int length, tcp::socket socket))
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
    thread_pool_size_(thread_pool_size), io_context_(io_context),
    inRequest_(inRequest)
{
    accept();
}

void TcpServer::run()
{
    // —оздаем пул потоков дл€ обработки соединений
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < thread_pool_size_; ++i)
    {
        threads.emplace_back([this]() { io_context_.run(); });
    }

    // ∆дем завершени€ всех потоков
    stop(threads);
}

void TcpServer::stop(std::vector<std::thread>& threads)
{
    for (std::size_t i = 0; i < thread_pool_size_; ++i)
    {
        threads[i].join();
    }
}

void TcpServer::accept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::cout << "New connection from: " << socket.remote_endpoint() << std::endl;
                std::make_shared<TcpSession>(std::move(socket), &inRequest_)->start();
            }

            accept();
        });
}