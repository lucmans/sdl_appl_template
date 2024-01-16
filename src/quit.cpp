#include "quit.hpp"

#include "logger.hpp"

#include <csignal>  // signal()
#include <cstdlib>  // exit()
#include <string>
#include <map>


namespace Quit {

// use this mapping instead of sigabbrev_np(), as the latter is not available on Windows
const std::map<const int, const std::string> signum_to_name = {
    {SIGINT, "SIGINT"},
    {SIGTERM, "SIGTERM"},
};


static volatile bool quit = false;


bool poll_quit() {
    return quit;
}


void set_quit() {
    Logger::info("Quitting application on next cycle...");
    quit = true;
}


void reset_quit() {
    quit = false;
}


void quit_signal_handler(const int signum) {
    if (quit) {
        if (signum_to_name.contains(signum))
            Logger::info("Received signal '" + std::string(signum_to_name.at(signum)) + "' while quitting; will now force quit");
        else
            Logger::info("Received signal number '" + std::to_string(signum) + "' while quitting; will now force quit");
        std::exit(-2);
    }

    if (signum_to_name.contains(signum))
        Logger::info("Signal '" + std::string(signum_to_name.at(signum)) + "' received");
    else
        Logger::info("Signal number '" + std::to_string(signum) + "' received");

    set_quit();
    Logger::hint("Sending another terminating signal will force quit");
}


void set_signal_handlers() {
    std::signal(SIGINT, quit_signal_handler);
    std::signal(SIGTERM, quit_signal_handler);
}

}  // namespace Quit
