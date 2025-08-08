
#include "collisions.hpp"
#include <cmath>  // for abs()

void checkCollisionsXY(tmx_map *map,
                       bool &floorCollision, bool &leftWallCollision,
                       bool &rightWallCollision, bool &ceilingCollision, bool &overlapping,
                       SDL_Rect &playerRect)
{
    floorCollision = leftWallCollision = rightWallCollision = ceilingCollision = overlapping = false;

    tmx_layer *layer = map->ly_head;

    while (layer) {
        if (!(layer->visible && layer->type == L_LAYER)) {
            layer = layer->next;
            continue;
        }

        unsigned int tileCount = map->width * map->height;

        for (unsigned int i = 0; i < tileCount; ++i) {
            unsigned int gid = layer->content.gids[i];
            tmx_tile *tile = tmx_get_tile(map, gid);
            if (!tile || !tile->collision) continue;

            int tileX = (i % map->width) * map->tile_width;
            int tileY = (i / map->width) * map->tile_height;

            tmx_object *obj = tile->collision;
            while (obj) {
                SDL_Rect objRect = {
                    tileX + (int)obj->x,
                    tileY + (int)obj->y,
                    (int)obj->width,
                    (int)obj->height
                };

                SDL_Rect shrunkPlayerRect = {
                    playerRect.x + 1,
                    playerRect.y + 1,
                    playerRect.w - 2,
                    playerRect.h - 2
                };

                if (SDL_HasIntersection(&shrunkPlayerRect, &objRect)) {
                    overlapping = true;

                    int playerCenterX = shrunkPlayerRect.x + shrunkPlayerRect.w / 2;
                    int playerCenterY = shrunkPlayerRect.y + shrunkPlayerRect.h / 2;
                    int objCenterX = objRect.x + objRect.w / 2;
                    int objCenterY = objRect.y + objRect.h / 2;

                    int deltaX = playerCenterX - objCenterX;
                    int deltaY = playerCenterY - objCenterY;

                    int halfWidths = (shrunkPlayerRect.w + objRect.w) / 2;
                    int halfHeights = (shrunkPlayerRect.h + objRect.h) / 2;

                    int overlapX = halfWidths - abs(deltaX);
                    int overlapY = halfHeights - abs(deltaY);

                    if (overlapX < overlapY) {
                        if (deltaX > 0) {
                            leftWallCollision = true;
                        } else {
                            rightWallCollision = true;
                        }
                    } else {
                        if (deltaY > 0) {
                            ceilingCollision = true;
                        } else {
                            floorCollision = true;
                        }
                    }
                } else {
                    // No intersection — but allow 2px vertical penetration into floor to count as floorCollision

                    int playerBottom = shrunkPlayerRect.y + shrunkPlayerRect.h;
                    int floorTop = objRect.y;

                    int playerCenterX = shrunkPlayerRect.x + shrunkPlayerRect.w / 2;

                    bool roughlyAlignedX = (playerCenterX + shrunkPlayerRect.w / 2) > objRect.x &&
                                           (playerCenterX - shrunkPlayerRect.w / 2) < (objRect.x + objRect.w);

                    if (!overlapping && roughlyAlignedX) {
                        // Check if playerBottom is inside floor collider by up to 1px
                        if (playerBottom >= floorTop - 1 && playerBottom <= floorTop) {
                            floorCollision = true;
                        }
                        // Or if player is up to 2px inside floor collider (penetration)
                        else if (playerBottom > floorTop && playerBottom <= floorTop + 2) {
                            floorCollision = true;
                        }
                    }
                }

                obj = obj->next;
            }
        }

        layer = layer->next;
    }
}
