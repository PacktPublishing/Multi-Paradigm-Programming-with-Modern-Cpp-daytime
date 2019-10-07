#include "daytime/server.h"

#include <boost/asio/ip/udp.hpp>
#include <chrono>
#include <ctime>
#include <string>

using namespace daytime;
using namespace boost::asio::ip;

namespace daytime::detail {
    // Get current time and format according to our protocol
    static std::string get_now_string(){
        using namespace std::chrono;
        auto now = system_clock::now();
        auto cnow = system_clock::to_time_t(now);
        return std::ctime(&cnow);
    }
}

struct server::impl{
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    std::array<char, 1> recv_buffer_;

    impl(boost::asio::io_context &io_context, short port)
    : socket_{io_context, udp::endpoint(udp::v4(), port)}
    {

    }

    void start_receive(){
        socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        [this](const boost::system::error_code &error, size_t){
            if (error && error != boost::asio::error::message_size)
                return;

            // This handler is invoked once the client connects
            // Make a date string and send it to them
            auto message = std::make_shared<std::string>(detail::get_now_string());
            // Capture message to ensure it lives long enough for data to be sent
            socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
            [message](const boost::system::error_code &, size_t) noexcept {
                // This handler is invoked when data has been sent
                // Do nothing; optionally log an error if send failed
            });

            // Keep listening
            start_receive();
        }
    );
    }
};
        
server::server(boost::asio::io_context &io_context, short port)
: impl_{std::make_unique<server::impl>(io_context, port)}{
    impl_->start_receive();
}

server::~server() = default;

int server::get_value() const {
    return 42;
}