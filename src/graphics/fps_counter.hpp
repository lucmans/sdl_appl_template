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
    const int height = 24;  // pixels

    double fps = -1.0;
    double unlocked_fps = -1.0;

    // set to 0 to turn off (255 for solid)
    uint8_t background_alpha = 75;
    int background_margin = 5;  // pixels
};


// forward declaration; definition in window.hpp
class Resolution;


class FpsCounter {
    public:
        FpsCounter(SDL_Renderer* const _renderer, TTF_Font* const _font);

        void render(const FpsCounterData& data, const Resolution& res);


    private:
        /* config */
        const SDL_Color text_color = {.r=0xff, .g=0xff, .b=0xff, .a=0xff};
        const uint fps_decimals = 3;  // TODO!


        /* member variables */
        SDL_Renderer* const renderer;
        TTF_Font* const font;

        TextTexture fps_text;
        TextTexture ufps_text;
};
