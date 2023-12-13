#ifndef ENGINE_HPP
#define ENGINE_HPP
#define SDL_MAIN_HANDLED

#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include "image.hpp"
#include "sprite.hpp"

#include <functional>
#include <string>
#include <vector>
#include <fstream>

typedef int TileArtDescriptor;
typedef int SpriteArtDescriptor;
typedef int SpriteDescriptor;

#define DEFAULT_FRAME_RATE_CAP 60

class Engine
{
public:
    Engine(const char *window_title, int window_height, int window_width);

    ~Engine();
    //
    const char *window_title;
    int window_height;
    int window_width;

    //
    static const int JUMP_VELOCITY = 10;
    static const int MAX_JUMP = 350;

    void addLayer(const char *filename);
    Image *getLayer(int layer);
    SDL_Rect setImageRenderArea(int, int, int, int);
    SDL_Rect setScreenRenderArea(int, int, int, int);

    SDL_Renderer *renderer;
    SDL_Window *window;

    struct Tile
    {
        SDL_Surface *surf;
        SDL_Texture *tex;
        int tileType;
        bool isWalkable;
        int ID;
    };

    void setRunning(int running)
    {
        switch (running)
        {
        case 1:
            isRunning = true;

            break;
        case 0:
            isRunning = false;
            break;
        }
    }

    bool Running()
    {
        return this->isRunning;
    }

    void setGridSize(int i) { this->gridSize = i; }
    int getGridSize() { return this->gridSize; }
    void save();

    void setRenderCopy(std::vector<Image *> &img, int x, int y, int width, int height, int ScreenWidth, int ScreenHeight);
    void setDeltaTime(double delt) { deltaTime = delt; }
    double returnDeltaTime() { return deltaTime; }

    void grid(int gridSize, int sWidth, int sHeight);
    void initializeTileMap(int gridSize, int sWidth, int sHeight);
    void renderTileMap();
    void changeTile(int i);
    void addTiles(const char *filename);
    void tilemap(int gridSize, int ScreenWidth, int ScreenHeight, int mouseX, int mouseY);
    bool resolveCollisions(Sprite *sprite);
    void applyGravity(Sprite *sprite);
    void moveRight(Sprite *sprite, int speed);
    void moveLeft(Sprite *sprite, int speed);
    void jump(Sprite *sprite, int height);
    void respawn(Sprite *sprite);
    void respawn_x(Sprite *sprite);
    void spawn(Sprite *sprite);
    void crouch(Sprite *sprite);
    std::vector<Image *> backgrounds;

    //================================
    // setters
    void set_backroundLocation(int x) { scroll_back_horizontal = x; }
    void set_backroundLocation2(int x) { scroll_back_horizontal2 = x; }
    void set_backroundLocation3(int x) { scroll_back_horizontal3 = x; }
    void set_backroundLocation4(int x) { scroll_back_horizontal4 = x; }
    void set_backroundLocation5(int x) { scroll_back_horizontal5 = x; }
    void set_Screen_width(int x) { Screen_width = x; }
    void set_Screen_height(int x) { Screen_height = x; }
    // getters

    int get_backroundLocation() { return scroll_back_horizontal; }
    int get_backroundLocation2() { return scroll_back_horizontal2; }
    int get_backroundLocation3() { return scroll_back_horizontal3; }
    int get_backroundLocation4() { return scroll_back_horizontal4; }
    int get_backroundLocation5() { return scroll_back_horizontal5; }
    int get_Screen_width() { return Screen_width; }
    int get_Screen_height() { return Screen_height; }

private:
    std::vector<Tile> tiles;
    std::vector<std::vector<int>> tileMap;
    Uint64 lastFrameTime;
    int selectedTileID;
    double gravity = 400;
    double deltaTime;
    int gridSize;
    int tileNum = 0;
    bool isRunning;
    int scroll_back_horizontal = 0;
    int scroll_back_horizontal2 = 0;
    int scroll_back_horizontal3 = 0;
    int scroll_back_horizontal4 = 0;
    int scroll_back_horizontal5 = 0;
    int Screen_width;
    int Screen_height;
};

#endif