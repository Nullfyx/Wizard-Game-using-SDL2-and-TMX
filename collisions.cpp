
#include "collisions.hpp"

const int CORNER_OVERLAP_THRESHOLD = 4;  // For blocking corners
const int STEP_HEIGHT = 8;               // Max step height allowed
const int SINK_TOLERANCE = 1;            // Ignore sinking ≤ 1px

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

// Helper: vertical overlap
int verticalOverlap(const SDL_Rect& a, const SDL_Rect& b) {
    int top = std::max(a.y, b.y);
    int bottom = std::min(a.y + a.h, b.y + b.h);
    return std::max(0, bottom - top);
}

// Checks tile collision + draws debug boxes
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

        // Red box: actual colliders
        SDL_Rect drawRect = {
            objRect.x - camera.x,
            objRect.y - camera.y,
            objRect.w,
            objRect.h
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

    // Predict future position
    SDL_Rect futureRect = playerRect;
    futureRect.x += (int)playerVelX;
    futureRect.y += (int)playerVelY;

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

    // FOOT RECT (for horizontal checks)
    SDL_Rect footRect = futureRect;
    footRect.y = futureRect.y + futureRect.h - STEP_HEIGHT;
    footRect.h = STEP_HEIGHT;

    // --- LEFT wall ---
    if (playerVelX < 0) {
        int tileX = futureRect.x / tileW;
        for (int y = 0; y < footRect.h; ++y) {
            int tileY = (footRect.y + y) / tileH;
            if (isSolidAt(tileX, tileY, futureRect)) {
                SDL_Rect tileRect = { tileX * tileW, tileY * tileH, tileW, tileH };

                int vOverlap = verticalOverlap(futureRect, tileRect);

                // Step tolerance
                int stepHeight = tileRect.y - (futureRect.y + futureRect.h);
                if (stepHeight >= -STEP_HEIGHT && stepHeight <= 0) continue;

                // Ignore tiny sinks
                if (vOverlap <= SINK_TOLERANCE) continue;

                if (vOverlap > CORNER_OVERLAP_THRESHOLD) {
                    leftWallCollision = true;
                    break;
                }
            }
        }
    }

    // --- RIGHT wall ---
    if (playerVelX > 0) {
        int tileX = (futureRect.x + futureRect.w - 1) / tileW;
        for (int y = 0; y < footRect.h; ++y) {
            int tileY = (footRect.y + y) / tileH;
            if (isSolidAt(tileX, tileY, futureRect)) {
                SDL_Rect tileRect = { tileX * tileW, tileY * tileH, tileW, tileH };

                int vOverlap = verticalOverlap(futureRect, tileRect);

                int stepHeight = tileRect.y - (futureRect.y + futureRect.h);
                if (stepHeight >= -STEP_HEIGHT && stepHeight <= 0) continue;

                if (vOverlap <= SINK_TOLERANCE) continue;

                if (vOverlap > CORNER_OVERLAP_THRESHOLD) {
                    rightWallCollision = true;
                    break;
                }
            }
        }
    }

    // --- FLOOR ---
    if (playerVelY > 0) {
        int testTileY = (futureRect.y + futureRect.h - 1) / tileH;
        for (int x = 1; x < futureRect.w; x += tileW / 2) {
            int tileX = (futureRect.x + x) / tileW;
            if (isSolidAt(tileX, testTileY, futureRect)) {
                floorCollision = true;
                break;
            }
        }
    }
    // --- CEILING ---
    else if (playerVelY < 0) {
        int testTileY = futureRect.y / tileH;
        for (int x = 1; x < futureRect.w; x += tileW / 2) {
            int tileX = (futureRect.x + x) / tileW;
            if (isSolidAt(tileX, testTileY, futureRect)) {
                ceilingCollision = true;
                break;
            }
        }
    }

    // --- Overlap check with current rect ---
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
