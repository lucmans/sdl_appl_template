#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>


// TODO: namespace

// maximizes and centers src in dst without changing src's aspect ratio
constexpr void fit_center(SDL_Rect& src, const SDL_Rect& dst, const int padding/* = 0*/);
