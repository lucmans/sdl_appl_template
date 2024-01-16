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
    const int fps_value = std::round(data.fps);
    TextTexture fps_value_text(renderer, std::to_string(fps_value), font, text_color);
    // TextTexture fps_value_text(renderer, std::to_string((int)std::round(data.unlocked_fps)), font, text_color);
    const double fps_text_ar = (double)fps_text.get_w() / fps_text.get_h();
    const int fps_text_width = data.text_height * fps_text_ar;
    const double fps_value_text_ar = (double)fps_value_text.get_w() / fps_value_text.get_h();
    const int fps_value_text_width = data.text_height * fps_value_text_ar;
    if (data.location == FpsCounterLocation::top_left) {
        fps_text_dst = {
            .x = 0,
            .y = 0,
            .w = fps_text_width,
            .h = data.text_height,
        };
        fps_value_dst = {
            .x = fps_text_width,
            .y = 0,
            .w = fps_value_text_width,
            .h = data.text_height,
        };
    }
    else if (data.location == FpsCounterLocation::top_right) {
        fps_text_dst = {
            .x = (res.w - fps_text_width) - fps_value_text_width,
            .y = 0,
            .w = fps_text_width,
            .h = data.text_height,
        };
        fps_value_dst = {
            .x = res.w - fps_value_text_width,
            .y = 0,
            .w = fps_value_text_width,
            .h = data.text_height,
        };
    }
    else if (data.location == FpsCounterLocation::bottom_left) {
        fps_text_dst = {
            .x = 0,
            .y = res.h - data.text_height,
            .w = fps_text_width,
            .h = data.text_height,
        };
        fps_value_dst = {
            .x = fps_text_width,
            .y = res.h - data.text_height,
            .w = fps_value_text_width,
            .h = data.text_height,
        };
    }
    else if (data.location == FpsCounterLocation::bottom_right) {
        fps_text_dst = {
            .x = (res.w - fps_text_width) - fps_value_text_width,
            .y = res.h - data.text_height,
            .w = fps_text_width,
            .h = data.text_height,
        };
        fps_value_dst = {
            .x = res.w - fps_value_text_width,
            .y = res.h - data.text_height,
            .w = fps_value_text_width,
            .h = data.text_height,
        };
    }
    else {
        Logger::error("Invalid FpsCounterLocation; defaulting to top_right");

        // default to top_right
        // TODO: refactor this repeated code
        fps_text_dst = {
            .x = (res.w - fps_text_width) - fps_value_text_width,
            .y = 0,
            .w = fps_text_width,
            .h = data.text_height,
        };
        fps_value_dst = {
            .x = res.w - fps_value_text_width,
            .y = 0,
            .w = fps_value_text_width,
            .h = data.text_height,
        };
    }

    // render shaded background
    if (data.background_alpha > 0) {
        SDL_Rect bg_dst = {
            .x = std::max(fps_text_dst.x - data.background_margin, 0),
            .y = std::max(fps_text_dst.y - data.background_margin, 0),
            .w = std::min(fps_text_dst.w + fps_value_dst.w + data.background_margin, res.w),
            .h = std::min(fps_text_dst.h + data.background_margin, res.h),
        };

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, data.background_alpha);
        SDL_RenderFillRect(renderer, &bg_dst);
    }

    SDL_RenderCopy(renderer, fps_text, NULL, &fps_text_dst);
    SDL_RenderCopy(renderer, fps_value_text, NULL, &fps_value_dst);
}
