
#include "collisions.hpp"
#include <iostream>
#include <tmx.h>
#include <cmath> // for fmod

// --- Helper: Get 1D tile index ---
int getTileIndex(tmx_map *map, int tileX, int tileY) {
    if (tileX < 0 || tileX >= (int)map->width || 
        tileY < 0 || tileY >= (int)map->height) {
        return -1;
    }
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
bool checkTileCollision(tmx_tile *tile, SDL_Rect &playerRect,
                        int tilePixelX, int tilePixelY,
                        int tileW, int tileH) {
    // Draw green debug box for all tiles checked
    SDL_Rect tileRect = {tilePixelX - camera.x, tilePixelY - camera.y, tileW, tileH};
    SDL_SetRenderDrawColor(wRenderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(wRenderer, &tileRect);

    if (!tile) return false;

    bool objectCollisionFound = false;

    // --- 1. Try collision object check first ---
    tmx_object *obj = tile->collision;
    while (obj) {
        SDL_Rect objRect = {
            tilePixelX + static_cast<int>(obj->x),
            tilePixelY + static_cast<int>(obj->y),
            static_cast<int>(obj->width),
            static_cast<int>(obj->height)
        };

        // Draw red debug box for collidable objects
        SDL_Rect drawRect = {
            objRect.x - camera.x,
            objRect.y - camera.y,
            objRect.w,
            objRect.h
        };
        SDL_SetRenderDrawColor(wRenderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(wRenderer, &drawRect);

        if (SDL_HasIntersection(&playerRect, &objRect)) {
            objectCollisionFound = true;
            break;
        }

        obj = obj->next;
    }

    if (objectCollisionFound) return true;

        return false;
}

// --- MAIN FUNCTION: Checks for tile collisions around player ---
void checkCollisionsXY(tmx_map *map,
                       bool &floorCollision, bool &leftWallCollision,
                       bool &rightWallCollision, bool &ceilingCollision, bool &overlapping,
                       SDL_Rect &playerRect)
{
    const int tileW = map->tile_width;
    const int tileH = map->tile_height;

    floorCollision = leftWallCollision = rightWallCollision = ceilingCollision = overlapping = false;

    // Predictive offsets (pixel perfect movement prediction)
    const int checkOffset = 1; // pixels ahead to test in each direction

    // Tile ranges from current position
    int leftTileX   = playerRect.x / tileW;
    int rightTileX  = (playerRect.x + playerRect.w - 1) / tileW;
    int topTileY    = playerRect.y / tileH;
    int bottomTileY = (playerRect.y + playerRect.h - 1) / tileH;
    int centerTileX = (playerRect.x + playerRect.w / 2) / tileW;
    int centerTileY = (playerRect.y + playerRect.h / 2) / tileH;

    tmx_layer *layer = map->ly_head;
    while (layer) {
        if (!(layer->visible && layer->type == L_LAYER)) {
            layer = layer->next;
            continue;
        }

        // --- LEFT wall check ---
        {
            SDL_Rect testRect = playerRect;
            testRect.x -= checkOffset; // simulate moving left
            tmx_tile *tileL = getTile(map, layer, leftTileX, centerTileY);
            if (checkTileCollision(tileL, testRect, leftTileX * tileW, centerTileY * tileH, tileW, tileH)) {
                float leftOverlap = (leftTileX * tileW + tileW) - testRect.x;
                if (leftOverlap > 1.0f) leftWallCollision = true;
            }
        }

        // --- RIGHT wall check ---
        {
            SDL_Rect testRect = playerRect;
            testRect.x += checkOffset; // simulate moving right
            tmx_tile *tileR = getTile(map, layer, rightTileX, centerTileY);
            if (checkTileCollision(tileR, testRect, rightTileX * tileW, centerTileY * tileH, tileW, tileH)) {
                float rightOverlap = (testRect.x + testRect.w) - (rightTileX * tileW);
                if (rightOverlap > 1.0f) rightWallCollision = true;
            }
        }

        // --- FLOOR check ---
        {
            SDL_Rect testRect = playerRect;
            testRect.y += checkOffset; // simulate moving down
            tmx_tile *tileD = getTile(map, layer, centerTileX, bottomTileY);
            if (checkTileCollision(tileD, testRect, centerTileX * tileW, bottomTileY * tileH, tileW, tileH)) {
                float bottomOverlap = (testRect.y + testRect.h) - (bottomTileY * tileH);
                if (bottomOverlap > 1.0f) {
                    floorCollision = true;
                    overlapping = true;
                }
            }
        }

        // --- CEILING check ---
        {
            SDL_Rect testRect = playerRect;
            testRect.y -= checkOffset; // simulate moving up
            tmx_tile *tileU = getTile(map, layer, centerTileX, topTileY);
            if (checkTileCollision(tileU, testRect, centerTileX * tileW, topTileY * tileH, tileW, tileH)) {
                float topOverlap = (topTileY * tileH + tileH) - testRect.x;
                if (topOverlap > 1.0f) ceilingCollision = true;
            }
        }

        layer = layer->next;
    }
}
