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

        // Print current time from time server
        daytime::client client {io_context, "hostname"};
        auto time = client.query_daytime();

        std::cout << "Current time from daytime server: "
                  << time
                  << std::endl;

        // Stop the IO context to end the program
        io_context.stop();
        thread.join();
    }
    catch (boost::system::system_error &ex){
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}