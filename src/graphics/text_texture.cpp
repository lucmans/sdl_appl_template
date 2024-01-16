#include "graphics/text_texture.hpp"

#include "exception.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


TextTexture::TextTexture(SDL_Renderer* const renderer, const std::string& text, TTF_Font* const font, const SDL_Color& color) {
    // SDL_Surface* text_surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Surface* text_surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (text_surface == NULL)
        throw Exception("Failed to render text to surface\nTTF error: " + std::string(TTF_GetError()));

    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);
    if (text_texture == NULL)
        throw Exception("Failed to create texture from surface\nSDL error: " + std::string(SDL_GetError()));

    SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
    SDL_SetTextureScaleMode(text_texture, SDL_ScaleModeBest);
}


TextTexture::~TextTexture() {
    SDL_DestroyTexture(text_texture);
}


SDL_Texture* TextTexture::get_texture() const noexcept {
    return text_texture;
}


TextTexture::operator SDL_Texture*() const noexcept {
    return text_texture;
}


int TextTexture::get_w() const noexcept {
    return w;
}


int TextTexture::get_h() const noexcept {
    return h;
}
