#include "engine.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

#define NO_FLAGS 0

Engine::Engine(const char* window_title, int window_height, int window_width) {
    int rendererFlags, windowFlags;
    rendererFlags = SDL_RENDERER_ACCELERATED;
    windowFlags = NO_FLAGS;

    if (SDL_Init(SDL_INIT_EVERYTHING)!= -1) 
    {
        this->window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_FULLSCREEN);
        if (this->window) 
        {
            //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
            this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
            if (this->renderer) 
            {
                std::cout << "Everything checks out" << std::endl;
            }
            else
            {
                SDL_DestroyWindow(this->window);
                SDL_Quit();
                std::cout<< "Failed to initialize SDL renderer: "<< SDL_GetError()<<std::endl;
                return;
            }
        }
        else
        {
            SDL_Quit();
            std::cout<< "Failed to initialize SDL window: "<< SDL_GetError()<<std::endl;
            return;
        }
    }
    else
    {
        std::cout<< "Failed to initialize SDL: "<< SDL_GetError()<<std::endl;
        return;
    }
    this->game_logic = game_logic;


    int imgFlags = IMG_INIT_WEBP | IMG_INIT_JPG | IMG_INIT_PNG;
    IMG_Init(imgFlags);
}

Engine::~Engine() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);

    for (TileArt ta : this->tile_arts) {
        SDL_DestroyTexture(ta);
    }

    SDL_Quit();
    IMG_Quit();
}

void Engine::Run() {
    SDL_Event event;

    while (true) {
        SDL_PollEvent(&event);
        SDL_RenderClear(this->renderer);

        DrawTile(0, 100, 100);

        switch (event.type) {
        case SDL_QUIT:
            return;
            break;
        default:
            this->game_logic(this);
            break;
        }

        SDL_RenderPresent(this->renderer);
    }
}

TileArtDescriptor Engine::LoadTileArt(std::string filename) 
{
    //SDL_Surface* this_image = IMG_Load(filename.c_str());
    // SDL_SetSurfaceBlendMode(this_image, SDL_BLENDMODE_BLEND);
    // SDL_Texture* this_texture = SDL_CreateTextureFromSurface(this->renderer, this_image);
    // if (this_image == NULL) 
    // {
    //     std::cout << "Failed to load tile at " << filename << std::endl;
    //     return -1;
    // }

    // this->tile_arts.push_back(this_image);
    // return this->tile_arts.size() - 1;
}

void Engine::DrawTile(TileArtDescriptor ta, int x, int y) {
    SDL_Texture* texture = tile_arts.at(ta);

    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    SDL_RenderCopy(this->renderer, texture, NULL, &dest);
}