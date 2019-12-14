#include "network/udp_server.h"
#include "network/detail/udp_server_impl.h"

using namespace boost::asio::ip;
using namespace network;
using namespace network::detail;

udp_server_impl::udp_server_impl(boost::asio::io_context &io_context, short port, size_t buffer_size)
    : socket_{io_context, udp::endpoint(udp::v4(), port)}
    , buffer_size_{buffer_size}
    { }

void udp_server_impl::start_receive_async(){
    stopped_ = false;

    std::string buff;
    buff.resize(buffer_size_, '\0');
    auto endpoint = std::make_unique<boost::asio::ip::udp::endpoint>();
    auto endpoint_ptr = endpoint.get();

    socket_.async_receive_from(
        boost::asio::buffer(buff), *endpoint_ptr,
        [this, buff, endpoint{std::move(endpoint)}]
        (const boost::system::error_code &error, size_t){

            // If there is an error, ask the implementation whether we want to continue
            auto invoked_error_callback = false;
            if (error && error != boost::asio::error::message_size) {
                if (!on_error(error)){
                    stop_receive_async();
                }
                invoked_error_callback = true;
            }

            // Resume listening before processing request (which might take a while)
            if (!stopped_)
                start_receive_async();

            if (!invoked_error_callback){
                // Let the implementation process the request
                on_receive(buff, *endpoint);
            }
        });
}

void udp_server_impl::send_message_async(std::shared_ptr<std::string> message,
                         const boost::asio::ip::udp::endpoint &remote_endpoint,
                         const std::function<void(boost::system::error_code)> &on_error){

    // Capture message to ensure it lives long enough for data to be sent
    socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint,
    [message](const boost::system::error_code &, size_t) noexcept {
        // This handler is invoked when data has been sent
        // Do nothing; optionally log an error if send failed
    });
}

void udp_server_impl::stop_receive_async(){
    stopped_ = true;
}

udp_server::udp_server(std::unique_ptr<detail::udp_server_impl> impl)
: impl_{std::move(impl)}
{}

void udp_server::start() {
    impl_->start_receive_async();
}

void udp_server::stop() {
    impl_->stop_receive_async();
}