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

class program_options{
    public:
        bool run_server = false;
        log_level log_level = log_level::default_level;

        program_options(int argc, char *argv[])
        {
            if (argc > 1)
            {
                for (std::string_view arg : std::span{&argv[1], static_cast<size_t>(argc - 1)})
                {
                    if (arg == "-server")
                        run_server = true;

                    if (arg == "-debug")
                        log_level = log_level::debug;
                }
            }
        }
};

class server_factory{
    public:
    static std::shared_ptr<network::server> create_server(short port, boost::asio::io_context &io_context){
        //TODO: as we add support for various servers, initialize them here
        auto server = std::make_shared<daytime::daytime_server> (io_context, port);
        all_servers_.emplace_back(server);
        return server;
    }

    static void stop_all_servers(){
        for (auto &srv: all_servers_){
            if (auto srv_shared_ptr = srv.lock()){
                srv_shared_ptr->stop();
            }
        }
    }
    private:
    static std::vector<std::weak_ptr<network::server>> all_servers_;
};
std::vector<std::weak_ptr<network::server>> server_factory::all_servers_;

logger create_logger(const program_options &options){
    auto log = logger::get_logger();
    log.set_level(options.log_level);
    return log;
}

class program{
    public:
    program(const program_options &options, const logger &log)
    : options_{options}
    , log_{log}
    {}

    int run()
    {
        try
        {
            run_io_context();
        }
        catch (std::exception &ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
            write_log(log_, log_level::error, "An exception has occurred: ", ex.what());
            return 1;
        }
        return 0;
    }

    private:
    void run_io_context(){
            // Initialize boost.asio; run the context in a dedicated thread
            boost::asio::io_context io_context;
            auto work = boost::asio::make_work_guard(io_context);
            std::thread thread{[&io_context]() { io_context.run(); }};

            // This guard will ensure that the thread is joined and context is stopped,
            // even in case of an exception
            helpers::scope_guard guard{[&thread, &io_context]() {
                io_context.stop();
                if (thread.joinable())
                    thread.join();
            }};

            if (options_.run_server)
                run_server(io_context);
            else
                run_client(io_context);
            // scope_guard will stop io_context and join the thread
    }

    void run_server(boost::asio::io_context &io_context)
    {

        write_log(log_, log_level::info, "Listening on port ", port);

        // Now we can construct any server type based on input arguments!
        auto server = server_factory::create_server(::port, io_context);
        server->do_on_error([log{this->log_}](const std::string &msg) mutable{
            write_log(log, log_level::error, msg);
        });
        server->start();

        std::cout << "Running in server mode. Press enter to exit";
        std::cin.get();

        server->stop();
        io_context.stop();
    }

    void run_client(boost::asio::io_context &io_context)
    {
        // Client mode: print current time from local server
        daytime::client client{io_context, "localhost", port};
        auto time = client.query_daytime();
        std::cout << "Current time from daytime server: "
                  << time
                  << std::endl;
    }

    program_options options_;
    logger log_;
};

int main(int argc, char *argv[])
{
    program_options options{argc, argv};
    auto log = create_logger(options);
    log.write(log_level::debug, "Starting up");

    auto log_messages = [log]() mutable {
        while(true){
            write_log(log, log_level::info, "Message from thread ", std::this_thread::get_id());
        }
    };

    for (int i = 0; i < 10; ++i){
        std::thread t(log_messages);
        t.detach();
    }

    return program{options, log}.run();
}