#pragma once

#include <exception>
#include <source_location>
#include <string>


class Exception : public std::exception {
    public:
        Exception(const std::string& _msg, const std::source_location& loc = std::source_location::current())
            : msg(print_src_location
                  ? _msg + "  (" + loc.file_name() + ":" + std::to_string(loc.line()) + ")"
                  : _msg) {}


        const char* what() const noexcept override {
            return msg.c_str();
        }


        /* config */
        static constexpr bool print_src_location = true;


    private:
        const std::string msg;
};
