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
        // contains the positioning logic for all GUI elements
        // called on resize to correctly scale the window
        void calculate_screen_coordinates(WindowData& window_data, const int res_w, const int res_h) const;

        Window(const std::string& title, const int _res_w, const int _res_h, WindowData& window_data);
        ~Window();

        // use through: (const) auto [w, h] = window.get_resolution();
        std::tuple<int, int> get_resolution() const;
        void set_resolution(const int w, const int h, WindowData& window_data);

        // only call once per prepare_frame() invocation
        void render_frame();

        void prepare_frame(const WindowData& window_data);


        /* config */
        static constexpr int DEFAULT_FONT_PT = 40;


    private:
        SDL_Window* sdl_window;
        Resolution resolution;
        SDL_Renderer* renderer;

        TTF_Font* default_font;

        std::unique_ptr<FpsCounter> fps_counter;
};
