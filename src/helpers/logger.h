#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>
#include <sstream>

namespace helpers {
    enum class log_level: int {
        off = 0,
        error,
        warning,
        info,
        debug
    };

    class logger {
        public:
        static logger get_logger();

        //! Write a message to the log
        void write(log_level level, const std::string &msg);

        log_level get_level() const;
        void set_level(log_level level);

        logger(const logger&) = default;
        logger& operator=(const logger&) = default;
        logger(logger&&) = delete;
        logger& operator=(logger&&) = delete;

        private:
        //! Use get_logger instead
        logger() = default;


        struct logger_impl;

        // All loggers use the same implementation
        std::shared_ptr<logger_impl> impl_;
    };

    namespace detail{
        template<typename Message>
        inline void stream_messages(std::stringstream &stream, const Message &msg) {
            stream << msg;
        }
        
        template<typename Message, typename... Messages>
        inline void stream_messages(std::stringstream &stream, const Message &msg, const Messages& ...more_msgs) {
            stream << msg;
            stream_messages(stream, more_msgs...);
        }
    }
    
    template<typename... Messages>
    inline void write_log(logger &logger, log_level level, const Messages& ...messages) {
        std::stringstream s;
        detail::stream_messages(s, messages...);
        logger.write(level, s.str());
    }
 
}

#endif