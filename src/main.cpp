#include "program.hpp"
#include "rsc_dir.hpp"
#include "quit.hpp"
#include "logger.hpp"
#include "profiling/startup_timer.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstdlib>  // EXIT_SUCCESS, EXIT_FAILURE
#include <sstream>
#include <iomanip>  // setprecision()


int main(int /*argc*/, char* /*argv*/[]) {
    // set-up quitting with ctrl+c in terminal
    Quit::set_signal_handlers();

    try {
        RscDir::set("rsc");
    }
    catch (const std::exception& e) {
        Logger::fatal("Failed to set resource directory");
        Logger::exception(e);
        return EXIT_FAILURE;
    }

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {  // also inits SDL_INIT_EVENTS
        Logger::fatal("SDL's video subsystem failed to initialize\nSDL error: " + std::string(SDL_GetError()));
        return EXIT_FAILURE;
    }
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        Logger::fatal("SDL's audio subsystem failed to initialize\nSDL error: " + std::string(SDL_GetError()));
        return EXIT_FAILURE;
    }
    if (TTF_Init() != 0) {
        Logger::fatal("SDL's TTF rendering engine failed to initialize\nTTF error: " + std::string(TTF_GetError()));
        return EXIT_FAILURE;
    }

    // init and run main program
    int ret = EXIT_SUCCESS;
    try {
        Program program;

        // print start-up time
        startup_timer.set_init_time();
        // format the startup time
        std::stringstream ss; ss << std::fixed << std::setprecision(3) << startup_timer.get_init_time();
        Logger::info("Start-up time: " + ss.str() + " ms");

        program.main_loop();
    }
    catch (const std::exception& e) {
        Logger::fatal("Uncaught exception!");
        Logger::exception(e);
        ret = EXIT_FAILURE;
    }

    // clean-up
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();

    return ret;
}
