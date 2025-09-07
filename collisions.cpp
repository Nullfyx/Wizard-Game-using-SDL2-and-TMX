#include "collisions.hpp"
#include "globals.hpp"
const int CORNER_OVERLAP_THRESHOLD = 4; // For blocking corners
const int STEP_HEIGHT = 8;              // Max step height allowed

// --- Helpers ---
int getTileIndex(tmx_map *map, int tileX, int tileY) {
  if (tileX < 0 || tileX >= (int)map->width || tileY < 0 ||
      tileY >= (int)map->height)
    return -1;
  return tileY * map->width + tileX;
}

tmx_tile *getTile(tmx_map *map, tmx_layer *layer, int tileX, int tileY) {
  int index = getTileIndex(map, tileX, tileY);
  if (index < 0)
    return nullptr;
  unsigned int gid = layer->content.gids[index];
  return tmx_get_tile(map, gid);
}

// --- Tile Collision Tester for edges/ground ---
bool rectHitsSolidTile(tmx_map *map, SDL_Rect &rect, bool ignorePlatforms) {
  const int tileW = map->tile_width;
  const int tileH = map->tile_height;
  int leftTileX = rect.x / tileW;
  int rightTileX = (rect.x + rect.w - 1) / tileW;
  int topTileY = rect.y / tileH;
  int bottomTileY = (rect.y + rect.h - 1) / tileH;
  for (int tx = leftTileX; tx <= rightTileX; ++tx) {
    for (int ty = topTileY; ty <= bottomTileY; ++ty) {
      tmx_layer *layer = map->ly_head;
      while (layer) {
        if (!layer->visible || layer->type != L_LAYER) {
          layer = layer->next;
          continue;
        }
        tmx_tile *tile = getTile(map, layer, tx, ty);
        if (!tile) {
          layer = layer->next;
          continue;
        }
        for (tmx_object *obj = tile->collision; obj; obj = obj->next) {
          SDL_Rect objRect = {tx * tileW + (int)obj->x,
                              ty * tileH + (int)obj->y, (int)obj->width,
                              (int)obj->height};
          if (SDL_HasIntersection(&rect, &objRect)) {
            const tmx_property *prop =
                tmx_get_property(tile->properties, "platform");
            bool isPlatform =
                prop && prop->type == PT_BOOL && prop->value.boolean;
            if (!isPlatform || !ignorePlatforms)
              return true;
          }
        }
        layer = layer->next;
      }
    }
  }
  return false;
}

// --- Main collision check: function signature unchanged ---
void checkCollisionsXY(tmx_map *map, bool &floorCollision,
                       bool &leftWallCollision, bool &rightWallCollision,
                       bool &ceilingCollision, bool &overlapping,
                       SDL_Rect &playerRect) {
  const int tileW = map->tile_width;
  const int tileH = map->tile_height;
  floorCollision = leftWallCollision = rightWallCollision = ceilingCollision =
      overlapping = false;

  // --- Left wall: 1px-wide probe left of player ---
  {
    SDL_Rect checkRect = {playerRect.x - 1, playerRect.y, 1, playerRect.h};
    leftWallCollision = rectHitsSolidTile(map, checkRect, true);
  }
  // --- Right wall: 1px-wide probe right of player ---
  {
    SDL_Rect checkRect = {playerRect.x + playerRect.w, playerRect.y, 1,
                          playerRect.h};
    rightWallCollision = rectHitsSolidTile(map, checkRect, true);
  }
  // --- Ceiling: 1px-tall strip above player ---
  {
    SDL_Rect checkRect = {playerRect.x, playerRect.y - 1, playerRect.w, 1};
    ceilingCollision = rectHitsSolidTile(map, checkRect, true);
  }
  // --- Floor: 1px-tall strip below player (platforms included) ---
  {
    SDL_Rect checkRect = {playerRect.x, playerRect.y + playerRect.h,
                          playerRect.w, 1};
    floorCollision = rectHitsSolidTile(map, checkRect, false);
  }
  // --- Overlap check: restore original "early exit" thoroughness ---
  {
    int leftTileX = playerRect.x / tileW;
    int rightTileX = (playerRect.x + playerRect.w - 1) / tileW;
    int topTileY = playerRect.y / tileH;
    int bottomTileY = (playerRect.y + playerRect.h - 1) / tileH;
    for (int tx = leftTileX; tx <= rightTileX; ++tx) {
      for (int ty = topTileY; ty <= bottomTileY; ++ty) {
        tmx_layer *layer = map->ly_head;
        while (layer) {
          if (!layer->visible || layer->type != L_LAYER) {
            layer = layer->next;
            continue;
          }
          tmx_tile *tile = getTile(map, layer, tx, ty);
          if (!tile) {
            layer = layer->next;
            continue;
          }
          for (tmx_object *obj = tile->collision; obj; obj = obj->next) {
            SDL_Rect objRect = {tx * tileW + (int)obj->x,
                                ty * tileH + (int)obj->y, (int)obj->width,
                                (int)obj->height};
            if (SDL_HasIntersection(&playerRect, &objRect)) {
              overlapping = true;
              return; // immediate exit (like OG logic!)
            }
          }
          layer = layer->next;
        }
      }
    }
  }
}
