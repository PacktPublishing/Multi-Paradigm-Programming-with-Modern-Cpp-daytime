#include "client.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/use_future.hpp>

using namespace daytime;

client::client(boost::asio::io_context &io_context, std::string server_hostname)
: hostname_{std::move(server_hostname)}
, io_context_{io_context} {

}

std::string client::query_daytime() const {

    // This is a modified example from Boost library, distributed under BSD license
    // You can find the original here
    // https://www.boost.org/doc/libs/1_71_0/doc/html/boost_asio/example/cpp11/futures/daytime_client.cpp
    using boost::asio::ip::udp;

    udp::resolver resolver(io_context_);

    std::future<udp::resolver::results_type> endpoints =
      resolver.async_resolve(
          udp::v4(), hostname_, "daytime",
          boost::asio::use_future);

    assert(endpoints.valid());

    // The async_resolve operation above returns the endpoints as a future
    // value that is not retrieved ...

    endpoints.wait();
    
    auto dns_result = endpoints.get();// ... until here. This call may block.
    if (dns_result.empty()){
        throw std::runtime_error("Could not resolve hostname " + hostname_);
    }

    udp::socket socket(io_context_, udp::v4());

    std::array<char, 1> send_buf  = {{ 0 }};
    std::future<std::size_t> send_length =
      socket.async_send_to(boost::asio::buffer(send_buf),
          *dns_result.begin(), 
          boost::asio::use_future);

    // Do other things here while the send completes.

    send_length.get(); // Blocks until the send is complete. Throws any errors.

    std::array<char, 128> recv_buf;
    udp::endpoint sender_endpoint;
    std::future<std::size_t> recv_length =
      socket.async_receive_from(
          boost::asio::buffer(recv_buf),
          sender_endpoint,
          boost::asio::use_future);

    // Do other things here while the receive completes.

    // Blocks until receive is complete.
    return std::string{recv_buf.data(), recv_length.get()};
}