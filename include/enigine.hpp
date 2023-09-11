#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <string>
#include <vector>

typedef int TileArtDescriptor;
typedef int SpriteArtDescriptor;
typedef int SpriteDescriptor;

#define DEFAULT_FRAME_RATE_CAP 60

class Engine {
public:
    Engine();

    TileArtDescriptor LoadTileArt(std::string filename);
    void PlaceTile(TileArtDescriptor tile, int x, int y, int layer_number);

    SpriteArtDescriptor LoadSprite(std::string filename);
    SpriteDescriptor CreateSprite(SpriteArtDescriptor sprite_art, int x, int y, int layer_number);
    void MoveSprite(SpriteDescriptor sprite, int x, int y);
    void ReplaceSpriteArt(SpriteDescriptor sprite, SpriteArtDescriptor new_sprite_art);
    void SetSpriteAnimation(SpriteDescriptor sprite, std::vector<SpriteArtDescriptor> art, unsigned animation_speed);

    int AddLayer(int tile_height, int tile_width);
    void ScrollLayer(int layer_number, int dx, int dy);
private:

    typedef struct {
        std::vector<Color> color_data;
        int width;
        int height;
    } TileArt;

    std::vector<TileArt> tile_arts;
    std::vector<std::vector<TileDescriptor>> layers;
};

#endif 