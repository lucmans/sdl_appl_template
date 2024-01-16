#include "window.hpp"

#include "rsc_dir.hpp"
#include "exception.hpp"
#include "logger.hpp"
#include "graphics/fps_counter.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <tuple>
#include <memory>  // make_unique()


void Window::calculate_screen_coordinates(WindowData& /*window_data*/, const int /*res_w*/, const int /*res_h*/) const {
    //
}


Window::Window(const std::string& title, const int res_w, const int res_h, WindowData& window_data)
    : resolution(res_w, res_h)
{
    uint32_t sdl_window_flags = 0;
    // if (cli_args.fullscreen)
        // sdl_window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;  // causes huge start-up time increase
    // sdl_window_flags |= SDL_WINDOW_UTILITY;
    // sdl_window_flags |= SDL_WINDOW_RESIZABLE;

    sdl_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution.w, resolution.h, sdl_window_flags);
    if (sdl_window == NULL)
        throw Exception("Failed to create window\nSDL error: " + std::string(SDL_GetError()));

    int w, h;
    SDL_GetWindowSize(sdl_window, &w, &h);
    if (resolution.w != w || resolution.h != h) {
        Logger::info("Resolution changed compared to start-up resolution (" + std::to_string(res_w) + "x" + std::to_string(res_h) + " -> " + std::to_string(w) + "x" + std::to_string(h) + ")");
        resolution.w = w;
        resolution.h = h;
    }

    renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED/* | SDL_RENDERER_PRESENTVSYNC*/);
    if (renderer == NULL) {
        SDL_DestroyWindow(sdl_window);
        throw Exception("Failed to create renderer for window\nSDL error: " + std::string(SDL_GetError()));
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    default_font = TTF_OpenFont((RscDir::get() / "font" / "DejaVuSans.ttf").string().c_str(), DEFAULT_FONT_PT);
    if (default_font == NULL) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(sdl_window);
        throw Exception("Failed to load font 'rsc/font/DejaVuSans.ttf'\nTTF error: " + std::string(TTF_GetError()));
    }

    // render initial black frame
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    fps_counter = std::make_unique<FpsCounter>(renderer, default_font);

    calculate_screen_coordinates(window_data, resolution.w, resolution.h);
}


Window::~Window() {
    // force clean-up before renderer
    fps_counter.reset();

    TTF_CloseFont(default_font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(sdl_window);
}


void Window::set_resolution(const int w, const int h, WindowData& window_data) {
    resolution.w = w;
    resolution.h = h;

    calculate_screen_coordinates(window_data, resolution.w, resolution.h);
}


std::tuple<int, int> Window::get_resolution() const {
    return {resolution.w, resolution.h};
}


void Window::render_frame() {
    SDL_RenderPresent(renderer);
}


void Window::prepare_frame(const WindowData& window_data) {
    // black background
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    fps_counter->render(window_data.fps_data, resolution);
}
