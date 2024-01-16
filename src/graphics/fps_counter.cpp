#include "graphics/fps_counter.hpp"

#include "window.hpp"
#include "logger.hpp"

#include "graphics/text_texture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <cmath>


FpsCounter::FpsCounter(SDL_Renderer* const _renderer, TTF_Font* const _font)
    : renderer(_renderer),
      font(_font),
      fps_text(renderer, "FPS: ", font, text_color),
      ufps_text(renderer, "uFPS: ", font, text_color)
{
    //
}


void FpsCounter::render(const FpsCounterData& data, const Resolution& res) {
    if (!data.show_fps)
        return;

    SDL_Rect fps_text_dst, fps_value_dst;
    // SDL_Rect ufps_text_dst, ufps_value_dst;

    // int h_offset = 0;
    TextTexture fps_value_text(renderer, std::to_string((int)std::round(data.fps)), font, text_color);
    if (data.location == FpsCounterLocation::top_left) {
        fps_text_dst = {
            .x = 0,
            .y = 0,
            .w = fps_text.get_w(),
            .h = fps_text.get_h()
        };
        fps_value_dst = {
            .x = fps_text.get_w(),
            .y = 0,
            .w = fps_value_text.get_w(),
            .h = fps_value_text.get_h()
        };
    }
    else if (data.location == FpsCounterLocation::top_right) {
        fps_text_dst = {
            .x = (res.w - fps_text.get_w()) - fps_value_text.get_w(),
            .y = 0,
            .w = fps_text.get_w(),
            .h = fps_text.get_h()
        };
        fps_value_dst = {
            .x = res.w - fps_value_text.get_w(),
            .y = 0,
            .w = fps_value_text.get_w(),
            .h = fps_value_text.get_h()
        };
    }
    else if (data.location == FpsCounterLocation::bottom_left) {
        fps_text_dst = {
            .x = 0,
            .y = res.h - fps_text.get_h(),
            .w = fps_text.get_w(),
            .h = fps_text.get_h()
        };
        fps_value_dst = {
            .x = fps_text.get_w(),
            .y = res.h - fps_value_text.get_h(),
            .w = fps_value_text.get_w(),
            .h = fps_value_text.get_h()
        };
    }
    else if (data.location == FpsCounterLocation::bottom_right) {
        fps_text_dst = {
            .x = (res.w - fps_text.get_w()) - fps_value_text.get_w(),
            .y = res.h - fps_text.get_h(),
            .w = fps_text.get_w(),
            .h = fps_text.get_h()
        };
        fps_value_dst = {
            .x = res.w - fps_value_text.get_w(),
            .y = res.h - fps_value_text.get_h(),
            .w = fps_value_text.get_w(),
            .h = fps_value_text.get_h()
        };
    }
    else {
        Logger::error("Invalid FpsCounterLocation; defaulting to top_right");

        // default to top_right
        // TODO: refactor this repeated code
        fps_text_dst = {
            .x = (res.w - fps_text.get_w()) - fps_value_text.get_w(),
            .y = 0,
            .w = fps_text.get_w(),
            .h = fps_text.get_h()
        };
        fps_value_dst = {
            .x = res.w - fps_value_text.get_w(),
            .y = 0,
            .w = fps_value_text.get_w(),
            .h = fps_value_text.get_h()
        };
    }

    // render shaded background
    if (data.background_alpha > 0) {
        SDL_Rect bg_dst = {
            .x = std::max(fps_text_dst.x - data.background_margin, 0),
            .y = std::max(fps_text_dst.y - data.background_margin, 0),
            .w = std::min(fps_text_dst.w + fps_value_dst.w + data.background_margin, res.w),
            .h = std::min(fps_text_dst.h + data.background_margin, res.h)
        };

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, data.background_alpha);
        SDL_RenderFillRect(renderer, &bg_dst);
    }

    SDL_RenderCopy(renderer, fps_text.get_texture(), NULL, &fps_text_dst);
    SDL_RenderCopy(renderer, fps_value_text.get_texture(), NULL, &fps_value_dst);
}
