#ifndef NETWORK_UDP_SERVER_H
#define NETWORK_UDP_SERVER_H

#include "network/server.h"
#include <memory>
#include <experimental/propagate_const>

// Forward declarations
namespace boost::asio{ 
    class io_context;
}

namespace network {
    namespace detail { class udp_server_impl; }

    class udp_server : public server {
        public:
        udp_server(std::unique_ptr<detail::udp_server_impl> impl);

        void start() override;
        void stop() override;

        protected:
        std::experimental::propagate_const<std::unique_ptr<detail::udp_server_impl>> impl_;
    };
}

#endif