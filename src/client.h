#ifndef DAYTIME_CLIENT_H
#define DAYTIME_CLIENT_H

#include <string>
#include <chrono>

// Forward declarations
namespace boost::asio{
    class io_context;
}

namespace daytime {

    // A simple daytime client
    class client {
        public:
        client(boost::asio::io_context &io_context, std::string server_hostname, short server_port);
        ~client() = default;

        client(const client &) = delete;
        client & operator=(const client &) = delete;
        client(client &&) = delete;
        client & operator =(client &&) = delete;

        // Get daytime from the server
        std::string query_daytime() const;

        private:
        boost::asio::io_context &io_context_;
        std::string hostname_;
        short port_;
    };
}

#endif