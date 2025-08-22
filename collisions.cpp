
#include "collisions.hpp"
#include "globals.hpp"

const int CORNER_OVERLAP_THRESHOLD = 4;  // For blocking corners
const int STEP_HEIGHT = 8;               // Max step height allowed

// --- Helpers ---
int getTileIndex(tmx_map *map, int tileX, int tileY) {
    if (tileX < 0 || tileX >= (int)map->width || tileY < 0 || tileY >= (int)map->height)
        return -1;
    return tileY * map->width + tileX;
}

tmx_tile* getTile(tmx_map *map, tmx_layer *layer, int tileX, int tileY) {
    int index = getTileIndex(map, tileX, tileY);
    if (index < 0) return nullptr;
    unsigned int gid = layer->content.gids[index];
    return tmx_get_tile(map, gid);
}

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
        if (SDL_HasIntersection(&testRect, &objRect))
            return true;
        obj = obj->next;
    }
    return false;
}

// ---------------- Main collision check ----------------
void checkCollisionsXY(tmx_map *map,
                       bool &floorCollision, bool &leftWallCollision,
                       bool &rightWallCollision, bool &ceilingCollision,
                       bool &overlapping, SDL_Rect &playerRect)
{
    const int tileW = map->tile_width;
    const int tileH = map->tile_height;

    floorCollision = leftWallCollision = rightWallCollision = ceilingCollision = overlapping = false;

    // --- Generic collision checker ---
    auto isSolidAtXY = [&](int tx, int ty, SDL_Rect &testRect, bool ignorePlatforms) {
        tmx_layer *layer = map->ly_head;
        while (layer) {
            if (!layer->visible || layer->type != L_LAYER) {
                layer = layer->next;
                continue;
            }

            tmx_tile* tile = getTile(map, layer, tx, ty);
            if (!tile) { layer = layer->next; continue; }

            if (checkTileCollision(tile, testRect, tx * tileW, ty * tileH, tileW, tileH)) {
                const tmx_property* prop = tmx_get_property(tile->properties, "platform");
                bool isPlatform = prop && prop->type == PT_BOOL && prop->value.boolean;

                if (!isPlatform || !ignorePlatforms)
                    return true; // solid collision
            }
            layer = layer->next;
        }
        return false;
    };

    // --- Probes helper ---
    auto probeTiles = [&](SDL_Rect &checkRect, bool horizontal, bool ignorePlatforms) {
        int probeCoords[3];
        if (horizontal) {
            probeCoords[0] = checkRect.y;
            probeCoords[1] = checkRect.y + checkRect.h / 2;
            probeCoords[2] = checkRect.y + checkRect.h - 1;
        } else {
            probeCoords[0] = checkRect.x;
            probeCoords[1] = checkRect.x + checkRect.w / 2;
            probeCoords[2] = checkRect.x + checkRect.w - 1;
        }

        for (int i = 0; i < 3; ++i) {
            int tileX = horizontal ? checkRect.x / tileW : probeCoords[i] / tileW;
            int tileY = horizontal ? probeCoords[i] / tileH : checkRect.y / tileH;
            if (isSolidAtXY(tileX, tileY, checkRect, ignorePlatforms))
                return true;
        }
        return false;
    };

    // --- Left wall ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.x -= 1;
        if (probeTiles(checkRect, true, true))
            leftWallCollision = true;
    }

    // --- Right wall ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.x += 1;
        checkRect.x += checkRect.w - 1;
        if (probeTiles(checkRect, true, true))
            rightWallCollision = true;
    }

    // --- Ceiling ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.y -= 1;
        if (probeTiles(checkRect, false, true))
            ceilingCollision = true;
    }

    // --- Floor (platforms block floor) ---
    {
        SDL_Rect checkRect = playerRect;
        checkRect.y += 1;
        checkRect.y += checkRect.h - 1;
        if (probeTiles(checkRect, false, false))
            floorCollision = true;
    }

    // --- Overlap with current rect ---
    {
        int leftTileX   = playerRect.x / tileW;
        int rightTileX  = (playerRect.x + playerRect.w - 1) / tileW;
        int topTileY    = playerRect.y / tileH;
        int bottomTileY = (playerRect.y + playerRect.h - 1) / tileH;

        for (int tx = leftTileX; tx <= rightTileX; ++tx) {
            for (int ty = topTileY; ty <= bottomTileY; ++ty) {
                if (isSolidAtXY(tx, ty, playerRect, false)) {
                    overlapping = true;
                    return;
                }
            }
        }
    }
}
