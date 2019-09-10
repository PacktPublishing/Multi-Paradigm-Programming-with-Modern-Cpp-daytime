#ifndef SERVER_H
#define SERVER_H

#include <boost/asio/ip/udp.hpp>

// Forward declarations
namespace boost::asio{ 
    class io_context;
}

namespace daytime {
    // A simple UDP daytime server
    class server {
        public:
        server(boost::asio::io_context &io_context, short port);
        ~server() = default;

        private:
        void start_receive();
        void handle_receive();
        void handle_send();

        server() = delete;
        server(const server&) = delete;
        server(server&&) = delete;
        server &operator = (const server&) = delete;
        server &operator = (server&&) = delete;

        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint remote_endpoint_;
        std::array<char, 1> recv_buffer_;
    };
}

#endif