#include "tcp_server.hpp"
#include "session.hpp"

#include <iostream>

#include <memory>
#include <utility>

using boost::asio::ip::tcp;


tcp_server::tcp_server(boost::asio::io_context &io_context, std::string host, unsigned short int port, boost::asio::thread_pool& threadpool, tbb::concurrent_unordered_set<std::string>& strings_set)
:acceptor(io_context, tcp::endpoint(boost::asio::ip::address::from_string(host), port)), threadpool(threadpool), strings_set(strings_set)
{
    std::cout << "Server listening on " << acceptor.local_endpoint().address().to_string() << ":" << acceptor.local_endpoint().port() << std::endl;
    do_accept();
}

void tcp_server::do_accept()
{
    acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<session>(std::move(socket), threadpool, strings_set)->start();
            }

            do_accept();
        });
}
