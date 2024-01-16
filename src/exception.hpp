#pragma once

#include <stdexcept>
#include <source_location>


class Exception : public std::exception {
    public:
        Exception(const std::string& _msg, const std::source_location& loc = std::source_location::current())
            : msg(print_src_location
                  ? _msg + "  (" + loc.file_name() + ":" + std::to_string(loc.line()) + ")"
                  : _msg) {}


        const char* what() const noexcept override {
            return msg.c_str();
        }


    private:
        /* config */
        const bool print_src_location = true;


        /* member variables */
        const std::string msg;
};
