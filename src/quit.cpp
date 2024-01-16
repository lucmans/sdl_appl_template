#include "quit.hpp"

#include "logger.hpp"

#include <csignal>  // catching signals
#include <cstring>  // sigabbrev_np()
#include <string>


namespace Quit {

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
        Logger::info("Received signal 'SIG" + std::string(sigabbrev_np(signum)) + "' while quitting; will now force quit");
        exit(-2);
    }

    Logger::info("Signal 'SIG" + std::string(sigabbrev_np(signum)) + "' received");

    set_quit();
    Logger::hint("Sending another terminating signal will force quit");
}


void set_signal_handlers() {
    signal(SIGINT, quit_signal_handler);
    signal(SIGTERM, quit_signal_handler);
}

}  // namespace Quit
