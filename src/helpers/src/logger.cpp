#include "logger.h"
#include <fstream>

namespace helpers{
    static std::string string_from_log_level(log_level l){
        const char *names[] = {
            "OFF", "ERROR", "WARNING", "INFO", "DEBUG"
        };
        return names[static_cast<int>(l)];
    }
    std::string get_now_string(){
        using namespace std::chrono;
        auto now = system_clock::now();
        auto cnow = system_clock::to_time_t(now);
        auto result = std::string{std::ctime(&cnow)};
        if (result.back() == '\n')
            result.pop_back();
        return result;
    }
}

using namespace helpers;

struct logger::logger_impl{
    log_level level = log_level::debug;
    std::ofstream stream;
};

logger logger::get_logger(){
    static logger singleton;
    if (!singleton.impl_){
        auto impl = std::make_shared<logger::logger_impl>();
        impl->stream.open("log.txt", std::ios_base::app);
        singleton.impl_ = impl;
    }
    return singleton;
}

void logger::write(log_level level, const std::string &message){
    if (level > impl_->level){
        return;
    }

    impl_->stream
        << "["
        << get_now_string()
        << "]\t"
        << string_from_log_level(level) 
        << '\t'
        << message
        << std::endl
        << std::flush;
}

log_level logger::get_level() const {
    return impl_->level;
}

void logger::set_level(log_level level) {
    impl_->level = level;
}