#include "helpers/scope_guard.h"
#include "daytime/client.h"
#include "daytime/daytime_server.h"
#include "helpers/logger.h"

#include <iostream>
#include <thread>
#include <span>
#include <string_view>

#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

using namespace helpers;

constexpr short port = 1234;

class server_factory{
    public:
    static std::shared_ptr<network::server> create_server(int argc, char *argv[], boost::asio::io_context &io_context){
        //TODO: as we add support for various servers, initialize them here
        auto server = std::make_shared<daytime::daytime_server> (io_context, port);
        all_servers_.emplace_back(server);
        return server;
    }

    static void stop_all_servers(){
        for (auto &srv: all_servers_){
            if (auto srv_shared_ptr = srv.lock()){
                srv->stop();
            }
        }
    }
    private:
    static std::vector<std::weak_ptr<network::server>> all_servers_;
};
std::vector<std::weak_ptr<network::server>> server_factory::all_servers_;

int main(int argc, char *argv[]){

    auto run_server = false;
    auto log = logger::get_logger();

    if (argc > 1)
    {
        for (std::string_view arg : std::span{&argv[1], argc - 1})
        {
            if (arg == "-server")
                run_server = true;

            if (arg == "-debug")
                log.set_level(log_level::debug);
        }
    }

    log.write(log_level::debug, "Starting up");


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

            // Now we can construct any server type based on input arguments!
            auto server = server_factory::create_server(argc, argv, io_context);
            server->start();

            std::cout << "Running in server mode. Press enter to exit";
            std::cin.get();

            server->stop();
            io_context.stop();

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