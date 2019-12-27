#ifndef SERVER_H
#define SERVER_H

#include "network/udp_server.h"

#include <memory>
#include <string>

// Forward declarations
namespace boost::asio{ 
    class io_context;
}

namespace daytime {
    namespace detail {
        class daytime_server_impl;
    }

    // A simple UDP daytime server
    class daytime_server final: public network::udp_server {
        public:
        daytime_server(boost::asio::io_context &io_context, short port);
        ~daytime_server() override;

        void set_response_format(std::string format);

        protected:
        // For derived classes that might want to add their own implementation
        daytime_server(std::unique_ptr<detail::daytime_server_impl> impl);

        private:
        // A helper to get implementation faster
        detail::daytime_server_impl &get_impl();
    };
}

#endif