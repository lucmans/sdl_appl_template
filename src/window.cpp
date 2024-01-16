#include "window.hpp"

#include "rsc_dir.hpp"
#include "exception.hpp"
#include "graphics/fps_counter.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <tuple>
#include <memory>


Window::Window(const std::string& title, const int res_w, const int res_h)
    : resolution(res_w, res_h)
{
    uint32_t sdl_window_flags;
    // if (cli_args.fullscreen)
    //     sdl_window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    // else
    sdl_window_flags = SDL_WINDOW_UTILITY/* | SDL_WINDOW_RESIZABLE*/;

    sdl_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution.w, resolution.h, sdl_window_flags);
    if (sdl_window == NULL)
        throw Exception("Failed to create window\nSDL error: " + std::string(SDL_GetError()));

    renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED/* | SDL_RENDERER_PRESENTVSYNC*/);
    if (renderer == NULL) {
        SDL_DestroyWindow(sdl_window);
        throw Exception("Failed to create renderer for window\nSDL error: " + std::string(SDL_GetError()));
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    default_font = TTF_OpenFont((RscDir::get() / "font" / "DejaVuSans.ttf").c_str(), DEFAULT_FONT_PT);
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
}


Window::~Window() {
    // force clean-up before renderer
    fps_counter.reset();

    TTF_CloseFont(default_font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(sdl_window);
}


void Window::set_resolution(const int w, const int h) {
    resolution.w = w;
    resolution.h = h;
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
