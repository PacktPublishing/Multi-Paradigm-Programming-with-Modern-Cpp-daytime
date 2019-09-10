#include "server.h"

#include <chrono>
#include <ctime>
#include <string>

using namespace daytime;
using namespace boost::asio::ip;

namespace daytime::detail {
    // Get current time and format according to our protocol
    std::string get_now_string(){
        using namespace std::chrono;
        auto now = system_clock::now();
        auto cnow = system_clock::to_time_t(now);
        return std::ctime(&cnow);
    }
}
        
server::server(boost::asio::io_context &io_context, short port)
: socket_{io_context, udp::endpoint(udp::v4(), port)} {
    start_receive();
}

void server::start_receive(){
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