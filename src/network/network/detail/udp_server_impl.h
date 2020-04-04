#ifndef NETWORK_SERVER_IMPL_H
#define NETWORK_SERVER_IMPL_H

#include <boost/asio/ip/udp.hpp>
#include <memory>
#include <vector>
#include <string>

namespace network::detail{
    class udp_server_impl {
        public:
        udp_server_impl(boost::asio::io_context &io_context, short port, size_t buffer_size);
        virtual ~udp_server_impl() = default;

        udp_server_impl(const udp_server_impl&) = delete;
        udp_server_impl(udp_server_impl&&) = delete;
        udp_server_impl &operator=(udp_server_impl&&) = delete;
        udp_server_impl &operator=(const udp_server_impl&) = delete;


        // Invoked on network error. Return true to continue listening, false to stop the server
        virtual bool on_error(const boost::system::error_code &error) = 0;

        // Invoked when the server receives a datagram.
        // Implementations must override this callback.
        // Implementations can write response directly into the socket or call send_message_async
        virtual void on_receive(
            const std::string &data,
            const boost::asio::ip::udp::endpoint &remote_endpoint) = 0;

        // Send a datagram to the remote endpoint
        void send_message_async(std::shared_ptr<std::string> message,
            const boost::asio::ip::udp::endpoint &remote_endpoint,
            const std::function<void(boost::system::error_code)> &on_error);

        void start_receive_async();
        void stop_receive_async();

        // Derived implementations can access the socket
        boost::asio::ip::udp::socket socket_;
        server::error_callback error_callback_;

        private:
        // We don't want implementations to change buffer size
        size_t buffer_size_;
        bool stopped_ = false;
    };
}

#endif