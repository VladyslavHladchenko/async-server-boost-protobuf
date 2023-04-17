#include "tcp_server.hpp"
#include "session.hpp"

#include <iostream>

#include <boost/thread/thread.hpp>
#include <tbb/concurrent_unordered_set.h>


int main(int argc, char* argv[])
{
    unsigned short int port = 8080;
    std::string host = "::";

    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    else if (argc == 3)
    {
        host = argv[1];
        port = atoi(argv[2]);
    }


    tbb::concurrent_unordered_set<std::string> strings_set;
    boost::asio::thread_pool threadpool(100);

    try
    {
        boost::asio::io_context io_context;

        tcp_server server(io_context, host, port, threadpool, strings_set);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    threadpool.join();
    return 0;
}
