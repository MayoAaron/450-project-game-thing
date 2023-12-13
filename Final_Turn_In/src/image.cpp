#include "image.hpp"
#include <iostream> // for std::runtime_error

Image::Image(const char *filename, SDL_Renderer *renderer)
{
    this->surface = IMG_Load(filename);
    if (!this->surface)
    {
        std::cout << "Failed to create Surface: " << SDL_GetError() << std::endl;
    }

    SDL_SetSurfaceBlendMode(this->surface, SDL_BLENDMODE_BLEND);

    this->texture = SDL_CreateTextureFromSurface(renderer, this->surface);
    if (!this->texture)
    {
        std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
}

Image::~Image()
{
    if (this->surface)
    {
        SDL_FreeSurface(this->surface);
        this->surface = nullptr;
    }

    if (this->texture)
    {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }
}
