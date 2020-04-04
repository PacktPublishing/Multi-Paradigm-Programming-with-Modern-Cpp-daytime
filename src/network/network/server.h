#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <functional>

namespace network{

    class server_impl;

    //! Abstract base server
    class server {
        public:
        server() = default;
        virtual ~server() = default;

        using error_callback = std::function<void(const std::string &)>;
        virtual void do_on_error(const error_callback &callback) = 0;

        server(const server&) = delete;
        server(server&&) = delete;
        server &operator = (const server&) = delete;
        server &operator = (server&&) = delete;

        /*! Begin listening.
            This call is non-blocking. Throws an exception if failed.
        */
        virtual void start() = 0;

        /*! Stop listening but don't release the resources yet.
            Subsequent call to start() must be possible.
        */
        virtual void stop() = 0;
        
    };
}

#endif