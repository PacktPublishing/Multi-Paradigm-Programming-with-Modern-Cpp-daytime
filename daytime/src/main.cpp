#include "scope_guard.h"
#include "client.h"
#include "server.h"

#include <iostream>
#include <thread>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

int main(int argc, char *argv[]){

    auto run_server = argc > 1 && std::string{argv[1]} == "-server";
    constexpr short port = 1234;

    try
    {
        // Initialize boost.asio; run the context in a dedicated thread
        boost::asio::io_context io_context;
        auto work = boost::asio::make_work_guard(io_context);
        std::thread thread{[&io_context](){ io_context.run(); }};

        // This guard will ensure that the thread is joined and context is stopped,
        // even in case of an exception
        helpers::scope_guard guard{ [&thread, &io_context](){
            io_context.stop();
            if (thread.joinable())
                thread.join();
        }};

        if (run_server){
            // Server mode; will run until process is killed
            daytime::server server {io_context, port};
            thread.join();
        }
        else {
            // Client mode: print current time from local server
            daytime::client client {io_context, "localhost", port};
            auto time = client.query_daytime();
            std::cout << "Current time from daytime server: "
                  << time
                  << std::endl;
        }
        // scope_guard will stop io_context and join the thread
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}