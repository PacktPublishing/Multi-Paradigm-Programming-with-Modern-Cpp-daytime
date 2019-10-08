#include "helpers/scope_guard.h"
#include "daytime/client.h"
#include "daytime/server.h"
#include "helpers/logger.h"

#include <iostream>
#include <thread>
#include <span>
#include <string_view>

#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

using namespace helpers;

int main(int argc, char *argv[]){

    auto run_server = false;
    auto log = logger::get_logger();

    if (argc > 1)
    {
        for (std::string_view arg : std::span{&argv[1], static_cast<size_t>(argc - 1)})
        {
            if (arg == "-server")
                run_server = true;

            if (arg == "-debug")
                log.set_level(log_level::debug);
        }
    }

    log.write(log_level::debug, "Starting up");

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

            write_log(log, log_level::info, "Listening on port ", port);

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
        write_log(log, log_level::error, "An exception has occurred: ", ex.what());
        return 1;
    }

    return 0;
}