#include "collisions.hpp"
#include <iostream>
#include <tmx.h>

// Helper: Get linear tile index
int getTileIndex(tmx_map *map, int tileX, int tileY) {
    if (tileX < 0 || tileX >= (int)map->width || tileY < 0 || tileY >= (int)map->height)
        return -1;
    return tileY * map->width + tileX;
}

// Helper: Get tile from layer using tile coords
tmx_tile* getTile(tmx_map *map, tmx_layer *layer, int tileX, int tileY) {
    int index = getTileIndex(map, tileX, tileY);
    if (index < 0) return nullptr;
    unsigned int gid = layer->content.gids[index];
    return tmx_get_tile(map, gid);
}

// Check if tile has "collidable" property
bool isCollidable(tmx_tile *tile) {
    if (!tile || !tile->properties) return false;

    const tmx_property *prop = tmx_get_property(tile->properties, "collidable");
    if (!prop) return false;

    if (prop->type == PT_BOOL) {
        return prop->value.boolean;
    } else if (prop->type == PT_STRING) {
        return (std::string(prop->value.string) == "true" || prop->value.string[0] == '1');
    }

    return false;
}

// Debug print for tile status
void printTileCollisionStatus(const std::string &label, tmx_tile *tile) {
    if (!tile) {
        std::cout << label << ": tile is null.\n";
        return;
    }
    std::cout << label << " collidable: " << (isCollidable(tile) ? "yes" : "no") << std::endl;
}

// Your main function: Check surroundings of the player
void checkCollisionsXY(tmx_map *map, int playerX, int playerY) {
    tmx_layer *layer = map->ly_head;

    while (layer) {
        int tileX = playerX / map->tile_width;
        int tileY = playerY / map->tile_height;

        // Print player info
        std::cout << "Checking collisions at tile (" << tileX << ", " << tileY << ")\n";
        std::cout << "Player pixel pos: (" << playerX << ", " << playerY << ")\n";

        // Get surrounding tiles
        tmx_tile *tileL = getTile(map, layer, tileX - 1, tileY);
        tmx_tile *tileR = getTile(map, layer, tileX + 1, tileY);
        tmx_tile *tileU = getTile(map, layer, tileX, tileY - 1);
        tmx_tile *tileD = getTile(map, layer, tileX, tileY + 1);

        // Print their collision status
        printTileCollisionStatus("Left", tileL);
        printTileCollisionStatus("Right", tileR);
        printTileCollisionStatus("Up", tileU);
        printTileCollisionStatus("Down", tileD);

        // Next layer
        layer = layer->next;
    }
}

