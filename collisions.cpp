#include "collisions.hpp"
#include <iostream>
#include <tmx.h>
#include <cmath> // for fmod

// --- Helper: Get 1D tile index ---
int getTileIndex(tmx_map *map, int tileX, int tileY) {
    if (tileX < 0 || tileX >= (int)map->width || tileY < 0 || tileY >= (int)map->height)
        return -1;
    return tileY * map->width + tileX;
}

// --- Helper: Get tile from layer by (x, y) tile position ---
tmx_tile* getTile(tmx_map *map, tmx_layer *layer, int tileX, int tileY) {
    int index = getTileIndex(map, tileX, tileY);
    if (index < 0) return nullptr;
    unsigned int gid = layer->content.gids[index];
    return tmx_get_tile(map, gid);
}

// --- Helper: Check if a tile has 'collidable' property ---
bool checkCollisionsXY(tmx_tile *tile) {
    if (!tile || !tile->properties) return false;

    const tmx_property *prop = tmx_get_property(tile->properties, "collidable");
    if (!prop) return false;

    if (prop->type == PT_BOOL) {
        return prop->value.boolean;
    } else if (prop->type == PT_STRING) {
        std::string val = prop->value.string;
        return val == "true" || val == "1";
    }

    return false;
}

// --- Helper: Print collidable status ---
void printTileCollisionStatus(const std::string &label, tmx_tile *tile) {
    std::cout << label << " collidable: " << (checkCollisionsXY(tile) ? "yes" : "no") << std::endl;
}

// --- MAIN FUNCTION: Checks for tile collisions around player ---

void checkCollisionsXY(tmx_map *map, int playerX, int playerY) {
    const int tileW = map->tile_width;
    const int tileH = map->tile_height;

    float centerX = playerX + tileW / 2.0f;
    float centerY = playerY + tileH / 2.0f;

    int tileX = static_cast<int>(centerX / tileW);
    int tileY = static_cast<int>(centerY / tileH);

    int bottomPixelY = playerY + tileH;
    int bottomTileY = bottomPixelY / tileH;

    int topPixelY = playerY;
    int topTileY = topPixelY / tileH;

    std::cout << "\n=== Collision Check ===\n";
    std::cout << "Player top-left: (" << playerX << ", " << playerY << ")\n";
    std::cout << "Tile at center:  (" << tileX << ", " << tileY << ")\n";
    std::cout << "Bottom tileY:    " << bottomTileY << "\n";

    tmx_layer *layer = map->ly_head;
    while (layer) {
        tmx_tile *tileL = getTile(map, layer, tileX - 1, tileY);
        tmx_tile *tileR = getTile(map, layer, tileX + 1, tileY);
        tmx_tile *tileU = getTile(map, layer, tileX, topTileY);
        tmx_tile *tileD = getTile(map, layer, tileX, bottomTileY);

        printTileCollisionStatus("Left", tileL);
        printTileCollisionStatus("Right", tileR);
        printTileCollisionStatus("Up", tileU);
        printTileCollisionStatus("Down", tileD);

        layer = layer->next;
    }
}
