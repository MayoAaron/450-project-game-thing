#ifndef IMAGE_HPP
#define IMAGE_HPP
#define SDL_MAIN_HANDLED

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class Image
{
private:
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect srcRect;
    SDL_Rect dstRect;

public:
    Image(const char *filename, SDL_Renderer *);
    ~Image();

    SDL_Surface *accessSurface()
    {
        return this->surface;
    }

    SDL_Texture *accessTexture()
    {
        return this->texture;
    }

    void setSrcRect(int x, int y, int width, int height)
    {
        SDL_Rect temp = {x, y, width, height};
        this->srcRect = temp;
    }

    void setDstRect(int x, int y, int width, int height)
    {
        SDL_Rect temp = {x, y, width, height};
        this->dstRect = temp;
    }

    SDL_Rect accessSrcRect()
    {
        return srcRect;
    }

    SDL_Rect accessDstRect()
    {
        return dstRect;
    }
};
#endif
