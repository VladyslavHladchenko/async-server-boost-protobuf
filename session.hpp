#ifndef SESSION_HPP
#define SESSION_HPP

#include "esw_server.pb.h"

#include <tbb/concurrent_unordered_set.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


class session : public std::enable_shared_from_this<session>
{
public:
    session(boost::asio::ip::tcp::socket socket, boost::asio::thread_pool& threadpool, tbb::concurrent_unordered_set<std::string>& strings_set);

    void start();

private:
    void do_read();

    void processRequest(const Request& request);

    void processGetCount();

    void processWords(const std::string& data);

    void processPostWords(const Request_PostWords& request_PostWords);

    void sendMessageSize(uint32_t size);

    template <typename T>
    void sendMessage(const T& message);

    void sendResponseOK();

    boost::asio::ip::tcp::socket socket_;
    uint32_t message_size;

    tbb::concurrent_unordered_set<std::string> &strings_set;
    boost::asio::thread_pool &threadpool;
};

#endif
