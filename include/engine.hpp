#ifndef ENGINE_HPP
#define ENGINE_HPP
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>

#include <functional>
#include <string>
#include <vector>

typedef int TileArtDescriptor;
typedef int SpriteArtDescriptor;
typedef int SpriteDescriptor;

#define DEFAULT_FRAME_RATE_CAP 60

class Engine {
public:
    Engine(const char* window_title, int window_height, int window_width, std::function<void(Engine*)> game_logic);
    ~Engine();

    TileArtDescriptor LoadTileArt(std::string filename);
    void PlaceTile(TileArtDescriptor tile, int x, int y, int layer_number);

    SpriteArtDescriptor LoadSprite(std::string filename);
    SpriteDescriptor CreateSprite(SpriteArtDescriptor sprite_art, int x, int y, int layer_number);
    void MoveSprite(SpriteDescriptor sprite, int x, int y);
    void ReplaceSpriteArt(SpriteDescriptor sprite, SpriteArtDescriptor new_sprite_art);
    void SetSpriteAnimation(SpriteDescriptor sprite, std::vector<SpriteArtDescriptor> art, unsigned animation_speed);

    int AddLayer(int tile_height, int tile_width);
    void ScrollLayer(int layer_number, int dx, int dy);


    void Run();
private:

    typedef SDL_Texture* TileArt;

    std::vector<TileArt> tile_arts;
    std::vector<std::vector<TileArtDescriptor>> layers;


    SDL_Renderer* renderer;
    SDL_Window* window;

    std::function<void(Engine*)> game_logic;
    
    void DrawTile(TileArtDescriptor ta, int x, int y);
};

#endif 