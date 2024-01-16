#pragma once

#include "graphics/fps_counter.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <tuple>
#include <memory>


struct WindowData {
    FpsCounterData fps_data;
};


struct Resolution {
    int w;
    int h;

    Resolution(const int _w, const int _h) : w(_w), h(_h) {};
};


class Window {
    public:
        Window(const std::string& title, const int _res_w, const int _res_h);
        ~Window();

        // use through: (const) auto [w, h] = window.get_resolution();
        std::tuple<int, int> get_resolution() const;
        void set_resolution(const int w, const int h);

        // only call once per prepare_frame() invocation
        void render_frame();

        void prepare_frame(const WindowData& window_data);


    private:
        /* config */
        const int DEFAULT_FONT_PT = 24;


        /* member variables */
        SDL_Window* sdl_window;
        Resolution resolution;
        SDL_Renderer* renderer;

        TTF_Font* default_font;

        std::unique_ptr<FpsCounter> fps_counter;
};
