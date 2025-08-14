#include "mapglobal.hpp"
#include <cstdint>
#include <tmx.h>
std::vector<moving_tile> enemies = {};
std::unordered_set<uint64_t> tileSet = {};
void build_enemies_from_map(tmx_map *map)
{
    auto layer = map->ly_head;
    if(layer->type == L_LAYER)
    {
	     unsigned int gid, x, y, w, h, flags;
    tmx_tileset *ts;
    tmx_image *im;
    void *image;
    float op = layer->opacity;

    for (int i = map->height - 1; i >= 0; --i) {
        for (int j = 0; j < map->width; ++j) {
            gid = (layer->content.gids[(i * map->width) + j]) & TMX_FLIP_BITS_REMOVAL;
            if (gid == 0 || gid >= map->tilecount) continue;
	           auto key = (((uint64_t)layer->id<<32)|((uint64_t)i<<16)|(uint64_t)j); 

            tmx_tile *tile = tmx_get_tile(map, gid);
            if (!tile) continue;

            // Animated tile remap
            if (tile->animation_len > 0) {
                anim_state *state = &animStates[gid];
                unsigned int local_anim_tile_id = tile->animation[state->current_frame].tile_id;

                // Find tileset list node that owns tile->tileset
                tmx_tileset_list *owner = map->ts_head;
                while (owner && owner->tileset != tile->tileset) owner = owner->next;
                if (!owner) continue;
                
                unsigned int first_gid = owner->firstgid;
                unsigned int current_gid = first_gid + local_anim_tile_id;
                tile = tmx_get_tile(map, current_gid);
		  // Initialize enemy moving tile (no drawing here)



    moving_tile e = {0};
e.x = j * ts->tile_width;
e.y = i * ts->tile_height;
    e.vx = 0.0f;              
    e.vy = 0.0f;
    e.width = 16;
    e.height = 16;
    e.anim.current_frame = 0;
    e.anim.time_acc = 0;
    e.physics.kxPos = e.x;
    e.physics.kyPos = e.y;
    e.physics.kvelocityX = e.vx;
    e.physics.kvelocityY = e.vy;
    e.physics.kminVelX = 0.6;
    e.id =key;
    tmx_tileset_list* tsl = map->ts_head;
    if (tsl) {
        e.ts = tsl->tileset;
        e.ts_firstgid = tsl->firstgid;
        e.base_local_id = 0;
 auto [it, inserted] = tileSet.insert((((uint64_t)layer->id<<32)|((uint64_t)i<<16)|(uint64_t)j)); if(inserted){ enemies.push_back(e); };

}

                if (!tile) continue;
            }

            ts = tile->tileset;
            im = tile->image;
            x = tile->ul_x;
            y = tile->ul_y;
            w = ts->tile_width;
            h = ts->tile_height;

            if (im) image = im->resource_image;
            else    image = ts->image->resource_image;

            flags = (layer->content.gids[(i * map->width) + j]) & ~TMX_FLIP_BITS_REMOVAL;

        }
    }

    }
    else{
	layer = layer->next;
    }
}
