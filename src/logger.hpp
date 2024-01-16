#pragma once

#include "profiling/startup_timer.hpp"

#include <iostream>
#include <source_location>
#include <string>

#include <unistd.h>  // isatty()


/*
 * Debug
 *     Information that is diagnostically helpful to people more than just developers (IT, sysadmins, etc.).
 * Info
 *     Generally useful information to log (service start/stop, configuration assumptions, etc).
 *     Info you want to always have available but usually don't care about under normal circumstances.
 * Warning
 *     Anything that can potentially cause application oddities, but which is automatically recovering (switching
 *     from a primary to backup server, retrying an operation, missing secondary data, etc.).
 * Error
 *     Any error which is fatal to the operation, but not the service or application (can't open a required file, missing data, etc.).
 *     These errors will force user (administrator, or direct user) intervention.
 * Fatal
 *     Any error forcing a shutdown of the service or application (to prevent data loss or further data loss).
 */


namespace Logger {

/* config */
// print time from program start in CLI messages
constexpr bool PRINT_PROGRAM_TIME = false;

// show source location of info messages
constexpr bool INFO_SOURCE_LOC = false;

// disable colours from being used in the terminal
constexpr bool NO_COLORS = false;


// constants for colour/format escape sequences
const std::string BLACK     = (NO_COLORS ? "" : "\033[30m");
const std::string RED       = (NO_COLORS ? "" : "\033[31m");
const std::string GREEN     = (NO_COLORS ? "" : "\033[32m");
const std::string YELLOW    = (NO_COLORS ? "" : "\033[33m");
const std::string BLUE      = (NO_COLORS ? "" : "\033[34m");
const std::string MAGENTA   = (NO_COLORS ? "" : "\033[35m");
const std::string CYAN      = (NO_COLORS ? "" : "\033[36m");
const std::string WHITE     = (NO_COLORS ? "" : "\033[37m");
const std::string BOLD      = (NO_COLORS ? "" : "\033[1m");
const std::string ITALIC    = (NO_COLORS ? "" : "\033[3m");
const std::string UNDERLINE = (NO_COLORS ? "" : "\033[4m");
const std::string RESET     = (NO_COLORS ? "" : "\033[0m");


// plain message
inline void msg(const std::string& msg_to_log) {
    std::cerr << msg_to_log << std::endl;
}


// without file and line number
inline void msg(const char* type, const std::string color, const std::string& msg) {
    if (isatty(STDERR_FILENO) == 1) {
        std::cerr << BOLD;
        if constexpr (PRINT_PROGRAM_TIME)
            std::cerr << "[" << startup_timer.get_program_time() / 1000.0 << "] ";

        std::cerr << color << type << RESET << ": " << msg << std::endl;
    }
    else {
        if constexpr (PRINT_PROGRAM_TIME)
            std::cerr << "[" << startup_timer.get_program_time() / 1000.0 << "] ";

        std::cerr << type << ": " << msg << std::endl;
    }
}


// with file and line number
inline void msg(const char* type, const std::string color, const char* file, const int line, const std::string& msg) {
    if (isatty(STDERR_FILENO) == 1) {
        std::cerr << BOLD;
        if constexpr (PRINT_PROGRAM_TIME)
            std::cerr << "[" << startup_timer.get_program_time() / 1000.0 << "] ";

        std::cerr << color << type << RESET << ": " << msg << "  (" << file << ":" << line << ")" << std::endl;
    }
    else {
        if constexpr (PRINT_PROGRAM_TIME)
            std::cerr << "[" << startup_timer.get_program_time() / 1000.0 << "] ";

        std::cerr << type << ": " << msg << "  (" << file << ":" << line << ")" << std::endl;
    }
}


inline void fatal(const std::string& msg, const std::source_location location = std::source_location::current()) {
    Logger::msg("Fatal", MAGENTA, location.file_name(), location.line(), msg);
}


inline void error(const std::string& msg, const std::source_location location = std::source_location::current()) {
    Logger::msg("Error", RED, location.file_name(), location.line(), msg);
}


inline void warning(const std::string& msg, const std::source_location location = std::source_location::current()) {
    Logger::msg("Warning", YELLOW, location.file_name(), location.line(), msg);
}


inline void info(const std::string& msg, const std::source_location location = std::source_location::current()) {
    if constexpr (INFO_SOURCE_LOC)
        Logger::msg("Info", GREEN, location.file_name(), location.line(), msg);
    else
        Logger::msg("Info", GREEN, msg);
}


inline void debug(const std::string& msg, const std::source_location location = std::source_location::current()) {
    Logger::msg("Debug", BLUE, location.file_name(), location.line(), msg);
}


inline void hint(const std::string& msg) {
    Logger::msg("Hint", CYAN, msg);
}


inline void exception(const std::exception& e) {
    Logger::msg("Exception", WHITE, e.what());
}

} // namespace Logger
