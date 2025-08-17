

#include "map.hpp"
#include "globals.hpp"
#include "mapglobal.hpp"
#include <string>
#include <cstdint>
#include <unordered_set>
anim_state animStates[MAX_TILES] = {};



void set_color(int color) {
    tmx_col_bytes col = tmx_col_to_bytes(color);
    SDL_SetRenderDrawColor(wRenderer, col.r, col.g, col.b, col.a);
}

void render_map(tmx_map *map) {
    set_color(map->backgroundcolor);
    draw_all_layers(map, map->ly_head);
}

void draw_all_layers(tmx_map *map, tmx_layer *layers) {
    while (layers) {
        if (layers->visible) {
            if (layers->type == L_GROUP) {
                draw_all_layers(map, layers->content.group_head);
            } else if (layers->type == L_OBJGR) {
                draw_objects(layers->content.objgr);
            } else if (layers->type == L_IMAGE) {
                draw_image_layer(layers->content.image);
            } else if (layers->type == L_LAYER) {
                draw_layer(map, layers);
            }
        }
        layers = layers->next;
    }
}

void draw_image_layer(tmx_image *image) {
    SDL_Rect dim;
    dim.x = dim.y = 0;
    SDL_Texture *texture = (SDL_Texture *)image->resource_image;
    SDL_QueryTexture(texture, NULL, NULL, &(dim.w), &(dim.h));
    SDL_RenderCopy(wRenderer, texture, NULL, &dim);
}

void draw_layer(tmx_map *map, tmx_layer *layer) {
    unsigned int gid, x, y, w, h, flags;
    tmx_tileset *ts;
    tmx_image *im;
    void *image;
    float op = layer->opacity;
    for (int i = map->height - 1; i >= 0; --i) {
        for (int j = 0; j < map->width; ++j) {
            gid = (layer->content.gids[(i * map->width) + j]) & TMX_FLIP_BITS_REMOVAL;
            if (gid == 0 || gid >= map->tilecount) continue;
    bool skip = false;

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
    e.physics.kminVelX = 0.5;
    uint64_t key = (((uint64_t)layer->id << 32) | ((uint64_t)i << 16) | (uint64_t)j);// Tileset info from map
    tmx_tileset_list* tsl = map->ts_head;
    if (tsl) {
        e.ts = tsl->tileset;
        e.ts_firstgid = tsl->firstgid;
        e.base_local_id = 0;
 if (tileSet.insert(key).second) { enemies.push_back(e); skip = true;}
   if (!tile || skip) continue;

}

                if (!tile || skip) continue;
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
           if(strcmp(tile->tileset->name, "enemies")) draw_tile(image, x, y, w, h,
                      j * ts->tile_width, i * ts->tile_height, op, flags);
        }
    }

}

void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags) {
    SDL_Rect src_rect, dest_rect;
    src_rect.x = sx;
    src_rect.y = sy;
    src_rect.w = sw;
    src_rect.h = sh;
    dest_rect.x = (int)(dx - camera.x);
    dest_rect.y = (int)(dy - camera.y);
    dest_rect.w = sw;
    dest_rect.h = sh;
    // Note: flags ignored; add flipping if needed
    SDL_RenderCopy(wRenderer, (SDL_Texture *)image, &src_rect, &dest_rect);
}

void draw_polyline(double **points, double x, double y, int pointsc) {
    for (int i = 1; i < pointsc; i++) {
        SDL_RenderDrawLine(wRenderer,
                           (int)(x + points[i - 1][0]), (int)(y + points[i - 1][1]),
                           (int)(x + points[i][0]),     (int)(y + points[i][1]));
    }
}

void draw_polygon(double **points, double x, double y, int pointsc) {
    draw_polyline(points, x, y, pointsc);
    if (pointsc > 2) {
        SDL_RenderDrawLine(wRenderer,
                           (int)(x + points[0][0]),            (int)(y + points[0][1]),
                           (int)(x + points[pointsc - 1][0]),  (int)(y + points[pointsc - 1][1]));
    }
}

void draw_objects(tmx_object_group *objgr) {
    SDL_Rect rect;
    set_color(objgr->color);
    tmx_object *head = objgr->head;
    while (head) {
        if (head->visible) {
            if (head->obj_type == OT_SQUARE) {
                rect.x = (int)((head->x - camera.x) * 4.0);
                rect.y = (int)((head->y - camera.y) * 4.0);
                rect.w = (int)(head->width * 4.0);
                rect.h = (int)(head->height * 4.0);
                SDL_RenderDrawRect(wRenderer, &rect);
            } else if (head->obj_type == OT_POLYGON) {
                draw_polygon(head->content.shape->points, head->x, head->y, head->content.shape->points_len);
            } else if (head->obj_type == OT_POLYLINE) {
                draw_polyline(head->content.shape->points, head->x, head->y, head->content.shape->points_len);
            } else if (head->obj_type == OT_ELLIPSE) {
                /* no ellipse in SDL2 */
            }
        }
        head = head->next;
    }
}

 
