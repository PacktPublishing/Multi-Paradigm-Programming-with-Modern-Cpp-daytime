#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <experimental/propagate_const>

// Forward declarations
namespace boost::asio{ 
    class io_context;
}

namespace daytime {
    // A simple UDP daytime server
    class server {
        public:
        server(boost::asio::io_context &io_context, short port);
        ~server();

        int get_value() const;

        private:

        server() = delete;
        server(const server&) = delete;
        server(server&&) = delete;
        server &operator = (const server&) = delete;
        server &operator = (server&&) = delete;

        struct impl;
        std::experimental::propagate_const<std::unique_ptr<impl>> impl_;
    };
}

#endif