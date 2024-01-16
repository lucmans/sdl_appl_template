#pragma once

#include "graphics/text_texture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


enum class FpsCounterLocation {
    top_left,
    top_right,
    bottom_left,
    bottom_right
};


struct FpsCounterData {
    bool show_fps = true;
    bool show_unlocked_fps = true;

    FpsCounterLocation location = FpsCounterLocation::top_right;
    int text_height = 30;  // pixels
    unsigned int fps_decimals = 3;  // TODO!

    double fps = -1.0;
    double unlocked_fps = -1.0;

    // set to 0 to turn off (255 for solid)
    uint8_t background_alpha = 75;
    int background_margin = 5;  // pixels
};


// forward declaration; definition in window.hpp
struct Resolution;


class FpsCounter {
    public:
        FpsCounter(SDL_Renderer* const _renderer, TTF_Font* const _font);

        void render(const FpsCounterData& data, const Resolution& res);


        /* config */
        static constexpr SDL_Color text_color = {.r=0xff, .g=0xff, .b=0xff, .a=0xff};


    private:
        SDL_Renderer* const renderer;
        TTF_Font* const font;

        TextTexture fps_text;
        TextTexture ufps_text;
};
