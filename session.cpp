#include "session.hpp"

#include <iostream>
#include <cctype>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/endian/conversion.hpp>

using boost::asio::ip::tcp;


static std::string decompress_gzip(const std::string &data)
{
    std::stringstream compressed(data);
    std::stringstream decompressed;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;

    out.push(boost::iostreams::gzip_decompressor());
    out.push(compressed);

    std::istream instream(&out);
    decompressed << instream.rdbuf();

    return decompressed.str();
}

session::session(tcp::socket socket, boost::asio::thread_pool& threadpool, tbb::concurrent_unordered_set<std::string>& strings_set)
:socket_(std::move(socket)), threadpool(threadpool), strings_set(strings_set)
{
}

void session::start()
{
    do_read();
}

void session::do_read()
{
    auto self(shared_from_this());

    socket_.async_read_some(boost::asio::buffer(reinterpret_cast<char *>(&message_size), 4),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                                if (!ec)
                                {
                                    boost::endian::big_to_native_inplace(message_size);

                                    char buffer_message[message_size];

                                    boost::asio::read(socket_, boost::asio::buffer(buffer_message, message_size));

                                    Request request;
                                    if (!request.ParseFromArray(buffer_message, message_size))
                                    {
                                        std::cerr << "Failed to parse Request" << std::endl;
                                        return;
                                    }

                                    processRequest(request);

                                    do_read();
                                }
                            });
}

void session::processRequest(const Request& request)
{
    if (request.has_getcount())
    {
        processGetCount();
    }
    else if (request.has_postwords())
    {
        processPostWords(request.postwords());
    }
}

void session::processGetCount()
{
    Response response(Response::default_instance());
    response.set_status(Response_Status::Response_Status_OK);
    response.set_counter(strings_set.size());

    strings_set.clear();

    sendMessage(response);
}

void session::processPostWords(const Request_PostWords &request_PostWords)
{
    boost::asio::post(threadpool, [this, request_PostWords, self = shared_from_this()]()
                      {
                        processWords(decompress_gzip( request_PostWords.data()));
                        sendResponseOK(); 
                      });
}

void session::processWords(const std::string& data)
{
    int last_whitespace_idx = -1;

    for (size_t i = 0; i < data.length(); i++)
    {
        if (iswspace(data[i]))
        {
            if (last_whitespace_idx + 1 < i)
            {
                strings_set.insert(data.substr(last_whitespace_idx + 1, i - last_whitespace_idx - 1));
            }
            last_whitespace_idx = i;
        }
    }

    if (last_whitespace_idx + 1 < data.length())
        strings_set.insert(data.substr(last_whitespace_idx + 1));
}



void session::sendMessageSize(uint32_t size)
{
    boost::endian::native_to_big_inplace(size);

    char buffer_int32[4] = {(char)(size), (char)(size >> 8), (char)(size >> 16), (char)(size >> 24)};

    socket_.write_some(boost::asio::buffer(buffer_int32, 4));
}

template <typename T>
void session::sendMessage(const T &message)
{
    sendMessageSize(message.ByteSize());

    char buffer_message[message.ByteSize()];

    message.SerializeToArray(buffer_message, message.ByteSize());
    socket_.write_some(boost::asio::buffer(buffer_message, message.ByteSize()));
}

void session::sendResponseOK()
{
    Response response(Response::default_instance());
    response.set_status(Response_Status::Response_Status_OK);

    sendMessage(response);
}