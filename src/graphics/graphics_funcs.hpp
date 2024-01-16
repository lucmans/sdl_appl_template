#pragma once

#include <SDL2/SDL.h>


// TODO: namespace

// maximizes and centers src in dst without changing src's aspect ratio
void fit_center(SDL_Rect& src, const SDL_Rect& dst, const int padding = 0);
