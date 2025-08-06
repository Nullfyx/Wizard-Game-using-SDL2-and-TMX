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
void setTileCollisionStatus(const std::string &label, tmx_tile *tile) {
    std::cout << label << " collidable: " << (checkCollisionsXY(tile) ? "yes" : "no") << std::endl;
}

// --- MAIN FUNCTION: Checks for tile collisions around player ---

void checkCollisionsXY(tmx_map *map, int playerX, int playerY,
                       bool &floorCollision, bool &leftWallCollision,
                       bool &rightWallCollision, bool &ceilingCollision, bool &overlapping) {
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

    int rightPixelX = playerX + tileW;
    int rightTileX = rightPixelX / tileW;

    int leftTileX = playerX / tileW;

    std::cout << "\n=== Collision Check ===\n";
    std::cout << "Player top-left: (" << playerX << ", " << playerY << ")\n";
    std::cout << "Tile at center:  (" << tileX << ", " << tileY << ")\n";
    std::cout << "Bottom tileY:    " << bottomTileY << "\n";

    floorCollision = false;
    leftWallCollision = false;
    rightWallCollision = false;
    ceilingCollision = false;

    tmx_layer *layer = map->ly_head;
    while (layer) {
        // LEFT WALL: Only if player's left side is actually in the tile
        if (playerX % tileW != 0) {
            tmx_tile *tileL = getTile(map, layer, leftTileX, tileY);
            setTileCollisionStatus("Left", tileL);
            if (checkCollisionsXY(tileL)) leftWallCollision = true;
        }

        // RIGHT WALL: Only if player's right side is in the next tile
        if (rightPixelX % tileW != 0) {
            tmx_tile *tileR = getTile(map, layer, rightTileX, tileY);
            setTileCollisionStatus("Right", tileR);
            if (checkCollisionsXY(tileR)) rightWallCollision = true;
        }

        // FLOOR
        tmx_tile *tileD = getTile(map, layer, tileX, bottomTileY);
        setTileCollisionStatus("Down", tileD);
        if (checkCollisionsXY(tileD)) floorCollision = true;

        // CEILING
        tmx_tile *tileU = getTile(map, layer, tileX, topTileY);
        setTileCollisionStatus("Up", tileU);
        if (checkCollisionsXY(tileU)) ceilingCollision = true;
	
	if (floorCollision) {
 	   // How far the bottom of the player is into the next tile (vertical overlap)
    	float bottomOverlap = fmod(playerY + tileH, tileH);
    
   	 // If there's a visible overlap into the solid tile (e.g., more than 1 pixel)
    	if (bottomOverlap > 1.0f) {
        	overlapping = true;
       		 std::cout << "Player is sinking into the floor by " << bottomOverlap << " pixels.\n";
    }
}

        layer = layer->next;
    }
}

