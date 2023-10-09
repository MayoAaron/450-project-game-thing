#include <iostream>
#include "engine.hpp"
#define ScreenWidth 1920
#define ScreenHeight 1080
#define ScrollSpeed 10

int main()
{
    Engine* game = new Engine("Game", 1920, 1080);
    
    //Creates a surface from the file specified
    SDL_Surface *image2 = IMG_Load("test_assets/Clouds3.png");
    if(!image2)
    {
        std::cout << "Failed to load image2 " << SDL_GetError()<<std::endl;
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        SDL_Quit();
        return 1;
    }
    
    //Creates a surface from the file specified
    SDL_Surface *image = IMG_Load("test_assets/Grassy_Gary2.png");
    if(!image)
    {
        std::cout << "Failed to load image " << SDL_GetError()<<std::endl;
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        SDL_Quit();
        return 1;
    }
    
    //Setting surface Blending modes
    SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceBlendMode(image2, SDL_BLENDMODE_BLEND);
    
    //Create Textures from the Surfaces
    SDL_Texture *tex2 = SDL_CreateTextureFromSurface(game->renderer, image2);
    SDL_Texture *tex1 = SDL_CreateTextureFromSurface(game->renderer, image);
//***********************************************************************************************Feature ^

    //Src defines which portion of the image to render, and dst defines which part of the screen to render it to, these are needed to move the image
    //The ground image had no rects, because it is static.
    SDL_Rect srcRect = {0,0,320,image2->h};
    SDL_Rect dstRect = {0,0,ScreenWidth,ScreenHeight};
    // SDL_Rect dstRect2 = {-dstRect.w,0,ScreenWidth,ScreenHeight}; //Copy of the first image, it's starting positon is set to the far  left of the screen for smooth scrolling
    // SDL_Rect dstRect3 = {ScreenWidth,0,ScreenWidth,ScreenHeight};
    
    SDL_UpdateWindowSurface(game->window);
    SDL_Event event;
    while(SDL_PollEvent(&event) >= 0)
    {
        switch(event.type)
        {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
                case SDLK_a:
                     srcRect.x += -ScrollSpeed; // Increase the x position to move the image right
                    //If the images leave the screan bounds they will go to the other side of the screen and start scrolling again
                    if(srcRect.x <= 0)
                        srcRect.x = image2->w-320;
                    break;
                case SDLK_d:
                    srcRect.x += ScrollSpeed; // Increase the x position to move the image right
                    //If the images leave the screan bounds they will go to the other side of the screen and start scrolling again
                    if(srcRect.x >= image2->w-320)
                        srcRect.x = 0;
                    break;
                case SDLK_LEFT: //Left arrow key
                     srcRect.x += -ScrollSpeed; // Increase the x position to move the image right
                    //If the images leave the screan bounds they will go to the other side of the screen and start scrolling again
                    if(srcRect.x <= 0)
                        srcRect.x = image2->w-320;
                    break;
                case SDLK_RIGHT: //Right arrow key
                    srcRect.x += ScrollSpeed; // Increase the x position to move the image right
                    //If the images leave the screan bounds they will go to the other side of the screen and start scrolling again
                    if(srcRect.x >= image2->w-320)
                        srcRect.x = 0;
                    break;
            }
            
        }
        //Clears the renderer, then copies the background and background copy to the render target, and then the foreground is copied.
        //Then it presents the render
        SDL_RenderClear(game->renderer);
        SDL_RenderCopy(game->renderer, tex2, &srcRect, &dstRect);
        SDL_RenderCopy(game->renderer, tex1, NULL, NULL);
        SDL_RenderPresent(game->renderer);
    }
    
    //Cleaning up all the variables once SDL_Quit() is called. Destroying the textures and surfaces, and then assigning all pointers used to nullptr
    SDL_DestroyTexture(tex1);
    SDL_DestroyRenderer(game->renderer);
    SDL_FreeSurface(image);
    SDL_FreeSurface(image2);
    SDL_DestroyWindow(game->window);
    
    game->window = nullptr;
    game->renderer = nullptr;
    image = nullptr;
    image2 = nullptr;
    
    SDL_Quit();
    return 0;
}