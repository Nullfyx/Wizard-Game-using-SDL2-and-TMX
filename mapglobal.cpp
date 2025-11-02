#include "mapglobal.hpp"
#include <cstring>
#include <iostream>

std::vector<moving_tile> enemies = {};
std::unordered_set<uint64_t> tileSet = {};
tmx_map *map = NULL;

void build_enemies_from_map(tmx_map *map) {
  if (!map)
    return;

  tmx_layer *layer = map->ly_head;
  while (layer) {
    if (layer->type == L_LAYER && strcmp(layer->name, "Tile Layer 1") == 0) {
      break;
    }
    layer = layer->next;
  }
  if (!layer || layer->type != L_LAYER)
    return;

  std::cout << layer->name << std::endl;

  tmx_tileset_list *firstTsl = map->ts_head;

  if (layer->type == L_LAYER) {
    int enemiesBuiltCount = 0;
    for (int i = map->height - 1; i >= 0; --i) {
      for (int j = 0; j < map->width; ++j) {
        unsigned int gid =
            (layer->content.gids[(i * map->width) + j]) & TMX_FLIP_BITS_REMOVAL;
        if (gid == 0 || gid >= MAX_TILES)
          continue;

        tmx_tile *baseTile = tmx_get_tile(map, gid);
        if (!baseTile)
          continue;

        if (baseTile->animation_len > 0) {
          tmx_tileset_list *owner = map->ts_head;
          while (owner && owner->tileset != baseTile->tileset)
            owner = owner->next;
          if (!owner)
            continue;

          unsigned int local_anim_tile_id = baseTile->animation[0].tile_id;
          unsigned int current_gid = owner->firstgid + local_anim_tile_id;
          tmx_tile *currentTile = tmx_get_tile(map, current_gid);
          if (!currentTile)
            continue;

          tmx_tileset *ts = currentTile->tileset;
          if (!ts)
            continue;

          moving_tile e = {0};
          e.width = ts->tile_width;
          e.height = ts->tile_height;
          e.x = j * (float)e.width;
          e.y = i * (float)e.height;
          e.physics.kxPos = e.x;
          e.physics.kyPos = e.y;
          e.physics.kminVelX = 0.6f;
          e.physics.timeOut = 0.5f;
          e.id =
              (((uint64_t)layer->id << 32) | ((uint64_t)i << 16) | (uint64_t)j);
          e.anim.current_frame = 0;
          e.anim.time_acc = 0;

          tmx_property *m = tmx_get_property(baseTile->properties, "mana");
          e.mana = (m && m->type == PT_FLOAT) ? m->value.decimal : 0.0f;

          if (firstTsl) {
            const char *imgSrc = nullptr;
            if (ts->image) {
              imgSrc = ts->image->source;
            } else if (firstTsl->tileset && firstTsl->tileset->image) {
              imgSrc = firstTsl->tileset->image->source;
            }

            if (imgSrc) {
              std::string fullPath(imgSrc);
              if (fullPath.substr(0, 3) == "../") {
                fullPath = fullPath.substr(3);
              }
              e.texture.WIMG_Load(fullPath);
              e.texture.setBlendMode(SDL_BLENDMODE_BLEND);
              e.texture.setCols(6);
              e.texture.setFPS(14);
              e.texture.setCells(6);
              e.ts = ts;
              e.ts_firstgid = owner->firstgid;
              e.base_local_id = baseTile->id;

              e.rect = {(int)e.x, (int)e.y, e.width, e.height};

              auto key = (((uint64_t)layer->id << 32) | ((uint64_t)i << 16) |
                          (uint64_t)j);
              auto [it, inserted] = tileSet.insert(key);
              if (inserted) {
                enemies.push_back(e);
                enemiesBuiltCount++;
              }
            }
          }
        }
      }
    }
    std::cout << "Built " << enemiesBuiltCount << " enemies\n";
  }
}
