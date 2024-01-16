#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>


// can directly be passed to any function requiring an `SDL_Texture*`
class TextTexture {
    public:
        // renders `text` to a texture using given font and color
        // throw exception on failure
        TextTexture(SDL_Renderer* const renderer, const std::string& text, TTF_Font* const font, const SDL_Color& color);
        ~TextTexture();

        // pointer is valid as long as the object on which get() was called remains alive
        // consider the return texture const (can't be const due to SDL)
        SDL_Texture* get_texture() const noexcept;
        // enable implicit cast to the underlying SDL_Texture in order to pass as `SDL_Texture*`
        operator SDL_Texture*() const noexcept;

        int get_w() const noexcept;
        int get_h() const noexcept;


    private:
        SDL_Texture* text_texture;
        int w, h;
};
