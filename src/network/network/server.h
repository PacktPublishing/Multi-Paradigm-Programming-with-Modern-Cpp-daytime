#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H


namespace network{

    class server_impl;

    //! Abstract base server
    class server {
        public:
        server() = default;
        virtual ~server() = default;

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