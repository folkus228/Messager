#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>

class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
    TcpSession(boost::asio::ip::tcp::socket socket,
        bool(*inRequest)(char* data, int length, boost::asio::ip::tcp::socket& socket));

    void start();

private:
    void read();

    void write(std::size_t length);

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    // возвращает True если надо отправить обратно буфер
    bool(*inRequest_)(char* data, int length, boost::asio::ip::tcp::socket& socket);
};

class TcpServer
{
public:
    TcpServer(boost::asio::io_context& io_context, short port, std::size_t thread_pool_size,
        bool(*inRequest)(char* data, int length, boost::asio::ip::tcp::socket& socket));

    void run();
    void stop(std::vector<std::thread>& threads);

private:
    void accept();

    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::size_t thread_pool_size_;
    bool(*inRequest_)(char* data, int length, boost::asio::ip::tcp::socket& socket);
};