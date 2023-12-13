#include "engine.hpp"
#include "sprite.hpp"

#include <iostream>

#define NO_FLAGS 0

Engine::Engine(const char *window_title, int window_width, int window_height)

{
    this->window_title = window_title;
    this->window_height = window_height;
    this->window_width = window_width;

    if (SDL_Init(SDL_INIT_EVERYTHING) != -1)
    {
        setRunning(1);

        this->window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_RESIZABLE);
        if (this->window)
        {
            // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
            this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
            set_Screen_width(window_width);
            set_Screen_height(window_height);

            if (this->renderer)
            {

                std::cout << "Everything checks out" << std::endl;
                std::cout << "height is" << this->window_height << std::endl;
                std::cout << "width is" << this->window_width << std::endl;
            }
            else
            {
                setRunning(0);
                SDL_DestroyWindow(this->window);
                SDL_Quit();
                std::cout << "Failed to initialize SDL renderer: " << SDL_GetError() << std::endl;
                return;
            }
        }
        else
        {
            setRunning(0);
            SDL_Quit();
            std::cout << "Failed to initialize SDL window: " << SDL_GetError() << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }
}

Engine::~Engine()
{
    for (Image *image : this->backgrounds)
    {
        SDL_DestroyTexture(image->accessTexture());
        SDL_FreeSurface(image->accessSurface());
    }
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
    IMG_Quit();
}

// add image and push it to backrounds vector
void Engine::addLayer(const char *filename)
{
    Image *image = new Image(filename, this->renderer);
    this->backgrounds.emplace_back(image);
}

// will be used to get the image that was pushed into the backrounds vector
Image *Engine::getLayer(int layer)
{
    return this->backgrounds.at(layer);
}

//*
SDL_Rect Engine::setImageRenderArea(int x, int y, int width, int height)
{
    SDL_Rect srcRect = {x, y, width, height};
    return srcRect;
}

//*
SDL_Rect Engine::setScreenRenderArea(int x, int y, int width, int height)
{
    SDL_Rect dstRect = {x, y, width, height};
    return dstRect;
}

// function to render image on screen
void Engine::setRenderCopy(std::vector<Image *> &img, int x, int y, int width, int height, int ScreenWidth, int ScreenHeight)
{
    // adding new position for x val to where the backround image will be rendered:
    img[0]->setSrcRect(get_backroundLocation(), y, width, height);
    img[1]->setSrcRect(get_backroundLocation2(), y, width, height);
    img[2]->setSrcRect(get_backroundLocation3(), y, width, height);
    img[3]->setSrcRect(get_backroundLocation4(), y, width, height);
    img[4]->setSrcRect(get_backroundLocation5(), y, width, height);

    // set the new location applicable to the users screen dimensions:
    img[0]->setDstRect(x, y, ScreenWidth, ScreenHeight);
    img[1]->setDstRect(x, y, ScreenWidth, ScreenHeight);
    img[2]->setDstRect(x, y, ScreenWidth, ScreenHeight);
    img[3]->setDstRect(x, y, ScreenWidth, ScreenHeight);
    img[4]->setDstRect(x, y, ScreenWidth, ScreenHeight);

    SDL_Rect src = img[0]->accessSrcRect();
    SDL_Rect dst = img[0]->accessDstRect();
    SDL_Rect src2 = img[1]->accessSrcRect();
    SDL_Rect dst2 = img[1]->accessDstRect();
    SDL_Rect src3 = img[2]->accessSrcRect();
    SDL_Rect dst3 = img[2]->accessDstRect();
    SDL_Rect src4 = img[3]->accessSrcRect();
    SDL_Rect dst4 = img[3]->accessDstRect();
    SDL_Rect src5 = img[4]->accessSrcRect();
    SDL_Rect dst5 = img[4]->accessDstRect();

    SDL_RenderCopy(this->renderer, img[0]->accessTexture(), &src, &dst);
    SDL_RenderCopy(this->renderer, img[1]->accessTexture(), &src2, &dst2);
    SDL_RenderCopy(this->renderer, img[2]->accessTexture(), &src3, &dst3);
    SDL_RenderCopy(this->renderer, img[3]->accessTexture(), &src4, &dst4);
    SDL_RenderCopy(this->renderer, img[4]->accessTexture(), &src5, &dst5);
}

// function to add appropriate image(s) to a tiles vector of Tile obj's
// it will contain the rendered texture of the given img
void Engine ::addTiles(const char *filename)
{
    Tile art;
    art.surf = IMG_Load(filename);
    art.tex = SDL_CreateTextureFromSurface(renderer, art.surf);
    SDL_FreeSurface(art.surf);
    tiles.emplace_back(art);
}

// renders grid on screen
void Engine ::grid(int gridSize, int ScreenWidth, int ScreenHeight)
{
    int maxRows = ScreenHeight / gridSize;

    int maxCols = ScreenWidth / gridSize;

    int adjustedGridSize = std::min(ScreenHeight / maxRows, ScreenWidth / maxCols);

    for (int row = 0; row < maxRows; ++row)
    {
        for (int col = 0; col < maxCols; ++col)
        {
            SDL_RenderDrawLine(renderer, col * adjustedGridSize, 0, col * adjustedGridSize, ScreenHeight);
            SDL_RenderDrawLine(renderer, 0, row * adjustedGridSize, ScreenWidth, row * adjustedGridSize);
        }
    }
}

// does proper initalization and manipulates the tileMap vector based on given parameters of the function
void Engine ::initializeTileMap(int gridSize, int sWidth, int sHeight)
{

    const int numRows = sHeight / gridSize;
    const int numCols = sWidth / gridSize;
    std::ifstream inputFile("save_data.txt");
    this->tileMap.assign(numRows, std::vector<int>(numCols, -1));
    if (inputFile.is_open())
    {
        for (int row = 0; row < numRows; ++row)
        {
            for (int col = 0; col < numCols; ++col)
            {
                inputFile >> tileMap[row][col];
            }
        }
        inputFile.close();
    }
}

void Engine ::tilemap(int gridSize, int ScreenWidth, int ScreenHeight, int mouseX, int mouseY)
{
    int numRows = ScreenHeight / gridSize;
    int numCols = ScreenWidth / gridSize;
    int cellX = mouseX / gridSize;
    int cellY = mouseY / gridSize;

    tileMap[cellY][cellX] = selectedTileID;
}

void Engine ::changeTile(int i)
{
    if (i >= 0 && i < tiles.size())
    {
        selectedTileID = i;
    }
}

void Engine ::renderTileMap()
{
    for (int row = 0; row < tileMap.size(); ++row)
    {
        for (int col = 0; col < tileMap[0].size(); ++col)
        {
            const int tileID = tileMap[row][col];
            if (tileID != -1)
            {
                SDL_Rect tileRect = {col * gridSize, row * gridSize, gridSize, gridSize};
                SDL_RenderCopy(renderer, tiles[tileNum].tex, NULL, &tileRect);
            }
        }
    }
}

void Engine ::save()
{
    std ::ofstream save_File("save_data.txt");
    if (save_File.is_open())
    {
        for (int row = 0; row < tileMap.size(); ++row)
        {
            for (int col = 0; col < tileMap[0].size(); ++col)
            {
                save_File << tileMap[row][col] << " ";
            }
        }
        save_File.close();
    }
}

bool Engine::resolveCollisions(Sprite *sprite)
{
    SDL_Rect spriteBoundingBox = sprite->accessToScreen();
    for (int row = 0; row < tileMap.size(); ++row)
    {
        for (int col = 0; col < tileMap[0].size(); ++col)
        {
            if (tileMap[row][col] != -1)
            {
                SDL_Rect tileBoundingBox = {col * gridSize, row * gridSize, gridSize, gridSize};
                SDL_Rect intersection;
                if (SDL_IntersectRect(&spriteBoundingBox, &tileBoundingBox, &intersection) == SDL_TRUE)
                {
                    // Collision detected

                    // Calculate the intersection area dimensions
                    int intersectionWidth = intersection.w;
                    int intersectionHeight = intersection.h;

                    // Determine collision on sides (left, right, top, bottom) of the sprite
                    int horizontalDistance = spriteBoundingBox.x - tileBoundingBox.x;
                    int verticalDistance = spriteBoundingBox.y - tileBoundingBox.y;

                    // Calculate the overlap on each side
                    int overlapRight = (spriteBoundingBox.x + spriteBoundingBox.w) - tileBoundingBox.x;
                    int overlapLeft = (tileBoundingBox.x + tileBoundingBox.w) - spriteBoundingBox.x;
                    int overlapTop = (tileBoundingBox.y + tileBoundingBox.h) - spriteBoundingBox.y;
                    int overlapBottom = (spriteBoundingBox.y + spriteBoundingBox.h) - tileBoundingBox.y;

                    // Determine which side(s) have collision and reverse momentum accordingly
                    if (overlapRight > 0 && overlapRight < overlapLeft && overlapRight < overlapTop && overlapRight < overlapBottom)
                    {
                        // Collision on the right side of the sprite
                        sprite->reverseHorizontalMomentum();
                    }
                    else if (overlapLeft > 0 && overlapLeft < overlapRight && overlapLeft < overlapTop && overlapLeft < overlapBottom)
                    {
                        // Collision on the left side of the sprite
                        sprite->reverseHorizontalMomentum();
                    }
                    else if (overlapTop > 0 && overlapTop < overlapRight && overlapTop < overlapLeft && overlapTop < overlapBottom)
                    {
                        // Collision on the top side of the sprite
                        sprite->reverseVerticalMomentum();
                    }
                    else if (overlapBottom > 0 && overlapBottom < overlapRight && overlapBottom < overlapLeft && overlapBottom < overlapTop)
                    {
                        // Collision on the bottom side of the sprite
                                }
                    return true; // Exit after the first collision detected
                }
            }
        }
    }
    return false;
}

void Engine ::applyGravity(Sprite *sprite)
{
    if (!resolveCollisions(sprite))
    {
        sprite->setyVelocity(gravity * deltaTime);
        sprite->spriteGrav();
        // std::cout << "MOVING!!" << std::endl;
    }
    else
        sprite->setyVelocity(0);
}

void Engine ::moveRight(Sprite *sprite, int speed)
{
    sprite->setxMaxSpeed(speed);
    sprite->spriteMove();
}

void Engine ::moveLeft(Sprite *sprite, int speed)
{
    sprite->setxMaxSpeed(-speed);
    sprite->spriteMove();
}

void Engine ::jump(Sprite *sprite, int height)
{
    if (resolveCollisions(sprite))
    {
        sprite->setyMaxSpeed(height);
        sprite->setJumpState();
    }
}
void Engine ::respawn(Sprite *sprite)
{
    sprite->spriteRespawn();
}
void Engine ::respawn_x(Sprite *sprite)
{
    sprite->set_animatex(0);
    sprite->x_spriteRespawn();
}
void Engine ::spawn(Sprite *sprite)
{
    std::cout << "the value of x animate is.." << sprite->get_animatex() << std::endl;
    sprite->selectSprite(0, 0, 128);
}
