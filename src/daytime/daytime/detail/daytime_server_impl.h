#ifndef DAYTIME_SERVER_IMPL
#define DAYTIME_SERVER_IMPL

#include "network/detail/udp_server_impl.h"

namespace daytime::detail {

    class daytime_server_impl : public network::detail::udp_server_impl {
        public:
        daytime_server_impl(boost::asio::io_context &io_context, short port);

        bool on_error(const boost::system::error_code &error) override;
        void on_receive(const std::string &, const boost::asio::ip::udp::endpoint &remote_endpoint) override;

        std::string response_format_;
    };
}

#endif