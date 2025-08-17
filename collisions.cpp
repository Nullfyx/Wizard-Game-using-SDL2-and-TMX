
#include "collisions.hpp"
#include "globals.hpp"

const int CORNER_OVERLAP_THRESHOLD = 4;  // For blocking corners
const int STEP_HEIGHT = 8;               // Max step height allowed

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

// Checks tile collision
bool checkTileCollision(tmx_tile *tile, SDL_Rect &testRect,
                        int tilePixelX, int tilePixelY,
                        int tileW, int tileH) {
    if (!tile) return false;

    tmx_object *obj = tile->collision;
    while (obj) {
        SDL_Rect objRect = {
            tilePixelX + (int)obj->x,
            tilePixelY + (int)obj->y,
            (int)obj->width,
            (int)obj->height
        };

        if (SDL_HasIntersection(&testRect, &objRect)) {

    return true;
}

	obj = obj->next;
    }
    return false;
}

// ---------------- Main collision check ----------------
void checkCollisionsXY(tmx_map *map,
                       bool &floorCollision, bool &leftWallCollision,
                       bool &rightWallCollision, bool &ceilingCollision, bool &overlapping,
                       SDL_Rect &playerRect)
{
    const int tileW = map->tile_width;
    const int tileH = map->tile_height;

    floorCollision = leftWallCollision = rightWallCollision = ceilingCollision = overlapping = false;

    // Solid tile check lambda
    auto isSolidAt = [&](int tx, int ty, SDL_Rect &testRect) {
        tmx_layer *layer = map->ly_head;
        while (layer) {
            if (layer->visible && layer->type == L_LAYER) {
                if (checkTileCollision(getTile(map, layer, tx, ty), testRect,
                                       tx * tileW, ty * tileH, tileW, tileH))
                    return true;
            }
            layer = layer->next;
        }
        return false;
    };

    // --- LEFT wall (1 pixel lookahead) ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.x -= 1;
        int tileX = checkRect.x / tileW;

        int probeYs[] = { checkRect.y, checkRect.y + checkRect.h / 2, checkRect.y + checkRect.h - 1 };
        for (int i = 0; i < 3; ++i) {
            int tileY = probeYs[i] / tileH;
            if (isSolidAt(tileX, tileY, checkRect)) {
                leftWallCollision = true;
                break;
            }
        }
    }

    // --- RIGHT wall (1 pixel lookahead) ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.x += 1;
        int tileX = (checkRect.x + checkRect.w - 1) / tileW;

        int probeYs[] = { checkRect.y, checkRect.y + checkRect.h / 2, checkRect.y + checkRect.h - 1 };
        for (int i = 0; i < 3; ++i) {
            int tileY = probeYs[i] / tileH;
            if (isSolidAt(tileX, tileY, checkRect)) {
                rightWallCollision = true;
                break;
            }
        }
    }

    // --- FLOOR (1 pixel lookahead down) ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.y += 1; // one pixel below current position
        int tileY = (checkRect.y + checkRect.h - 1) / tileH;

        int probeXs[] = { checkRect.x, checkRect.x + checkRect.w / 2, checkRect.x + checkRect.w - 1 };
        for (int i = 0; i < 3; ++i) {
            int tileX = probeXs[i] / tileW;
            if (isSolidAt(tileX, tileY, checkRect)) {
                floorCollision = true;
                break;
            }
        }
    }

    // --- CEILING (1 pixel lookahead up) ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.y -= 1;
        int tileY = checkRect.y / tileH;

        int probeXs[] = { checkRect.x, checkRect.x + checkRect.w / 2, checkRect.x + checkRect.w - 1 };
        for (int i = 0; i < 3; ++i) {
            int tileX = probeXs[i] / tileW;
            if (isSolidAt(tileX, tileY, checkRect)) {
                ceilingCollision = true;
                break;
            }
        }
    }

    // --- Overlap check with current rect ---
    {
        int leftTileX   = playerRect.x / tileW;
        int rightTileX  = (playerRect.x + playerRect.w - 1) / tileW;
        int topTileY    = playerRect.y / tileH;
        int bottomTileY = (playerRect.y + playerRect.h - 1) / tileH;

        for (int tx = leftTileX; tx <= rightTileX; ++tx) {
            for (int ty = topTileY; ty <= bottomTileY; ++ty) {
                if (isSolidAt(tx, ty, playerRect)) {
                    overlapping = true;
                    return;
                }
            }
        }
    }
} 
