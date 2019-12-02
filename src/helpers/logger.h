#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <sstream>
#include <string>

namespace helpers {

    enum class log_level: int {
        off = 0,
        error,
        warning,
        info,
        default_level = info,
        debug,
        _last
    };

    class logger {
        public:
        //! Get the logger singleton, initialized on first call
        static logger get_logger();

        // Copying the logger is cheap, thanks to shared implementation
        logger(const logger&) = default;
        logger& operator=(const logger&) = default;

        ~logger() = default;

        // Move leaves the object in a 'default' state,
        // which does not make sense for a logger
        logger(logger&&) = delete;
        logger& operator=(logger&&) = delete;

        //! Get current log level
        log_level get_level() const;
        //! Messages with higher level will be ignored by the logger
        void set_level(log_level level);

        //! Write a message to the log, unless the current log level is lower
        void write(log_level level, const std::string &msg) const;

        private:
        // Use get_logger instead
        logger() = default;

        // All loggers use the same implementation
        struct logger_impl;
        std::shared_ptr<logger_impl> impl_;
    };

    namespace detail{
        template<typename MessagePart>
        inline void stream_parts(std::stringstream &stream, const MessagePart &part) {
            stream << part;
        }
        
        template<typename MessagePart, typename... MessageParts>
        inline void stream_parts(std::stringstream &stream, const MessagePart &part, const MessageParts& ...more_parts) {
            stream << part;
            stream_parts(stream, more_parts...);
        }
    }
    
    /*! Write any amount of elements of various types into a single log entry
        Every part type must support stream operator <<
    */
    template<typename... MessageParts>
    inline void write_log(logger &logger, log_level level, const MessageParts& ...parts) {
        // Optimization: check level before doing expensive conversions
        if (level > logger.get_level())
            return;

        // Use string stream to convert
        std::stringstream s;
        detail::stream_parts(s, parts...);
        logger.write(level, s.str());
    }
}

#endif