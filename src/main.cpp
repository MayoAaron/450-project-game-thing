#include "engine.hpp"
#include <iostream>

void GameLogic(Engine* e) {

}

int main() {
    Engine e("Test window", 600, 800, GameLogic);
    TileArtDescriptor ta = e.LoadTileArt("test_assets/dirty_mike.bmp");

    e.Run();
    return 0;
}