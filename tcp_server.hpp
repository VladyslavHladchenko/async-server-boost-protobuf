#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <boost/asio.hpp>

#include <tbb/concurrent_unordered_set.h>
#include <boost/thread/thread.hpp>


class tcp_server
{
public:
    tcp_server(boost::asio::io_context &io_context, std::string host, unsigned short int port, boost::asio::thread_pool& threadpool, tbb::concurrent_unordered_set<std::string>& strings_set);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor;
    tbb::concurrent_unordered_set<std::string>& strings_set;
    boost::asio::thread_pool& threadpool;
};


#endif
