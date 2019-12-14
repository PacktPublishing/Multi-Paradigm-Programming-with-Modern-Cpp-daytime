#include "daytime/daytime_server.h"
#include "daytime/detail/daytime_server_impl.h"
#include "network/udp_server.h"

using namespace daytime;
using namespace boost::asio::ip;
using namespace daytime::detail;

daytime_server_impl::daytime_server_impl(boost::asio::io_context &io_context, short port)
: network::detail::udp_server_impl(io_context, port, 1)
{
}

bool daytime_server_impl::on_error(const boost::system::error_code &error){
    // always stop on error
    return false;
}

void daytime_server_impl::on_receive(const std::string &, const boost::asio::ip::udp::endpoint &remote_endpoint){

    // Make a date string and send it to the client
    auto message = std::make_shared<std::string>(get_now_string());
    send_message_async(message, remote_endpoint, [](const boost::system::error_code &) noexcept {
        //TODO: add error handling
    } );
}

std::string daytime_server_impl::get_now_string() const{
    if (!response_format_.empty())
        throw std::runtime_error("Custom format is not implemented");

    using namespace std::chrono;
    auto now = system_clock::now();
    auto cnow = system_clock::to_time_t(now);
    return std::ctime(&cnow);
}


daytime_server::daytime_server(boost::asio::io_context &io_context, short port)
: daytime_server(std::make_unique<daytime_server_impl>(io_context, port))
{
}

daytime_server::daytime_server(std::unique_ptr<daytime_server_impl> impl)
: network::udp_server(std::move(impl))
{
}

daytime_server::~daytime_server() = default;

daytime_server_impl &daytime_server::get_impl() {
    // static_cast is safe because constructors only allow daytime_server_impl
    // inline the function for even better performance (although the compiler should optimize)
    auto &impl = static_cast<daytime_server_impl&>(*impl_);
    return impl;
}

void daytime_server::set_response_format(std::string format){
    get_impl().response_format_ = format;
}
