#include "scope_guard.h"
#include "client.h"

#include <iostream>
#include <thread>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

int main(int, char**){

    try
    {
        // Initialize boost.asio; run the context in a dedicated thread
        boost::asio::io_context io_context;
        auto work = boost::asio::make_work_guard(io_context);
        std::thread thread{[&io_context](){ io_context.run(); }};
        helpers::scope_guard guard{ [&thread, &io_context](){
            io_context.stop();
            thread.join();
        }};

        // Print current time from time server
        daytime::client client {io_context, "tick.gatech.edu"};
        auto time = client.query_daytime();

        std::cout << "Current time from daytime server: "
                  << time
                  << std::endl;

        // scope_guard will stop io_context and join the thread
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}