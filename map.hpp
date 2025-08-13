#ifndef MAP_HPP
#define MAP_HPP

#include <SDL2/SDL.h>
#include <tmx.h>
#include <cstring>
#include "globals.hpp"
#include <iostream>
#include "player.hpp"
#include "Kinematics.hpp"

using namespace std;

// Renderer used globally (defined elsewhere)
extern SDL_Renderer *wRenderer;

constexpr int MAX_TILES = 10000;

typedef struct {
    int current_frame;
    unsigned int time_acc;
} anim_state;

// A lightweight movable entity that uses a tileset tile/animation for visuals
typedef struct {
    float x, y;                // world position in pixels
    float vx, vy;              // velocity in px/s
    anim_state anim;           // animation state
    tmx_tileset *ts;           // owning tileset pointer
    unsigned int ts_firstgid;  // tileset’s firstgid (from tmx_tileset_list)
    unsigned int base_local_id;// base local tile id in the tileset (the tile holding animation)
			       //
    Kinematics physics;
    void update();
    void enemyUpdate(float deltaTime, tmx_map* map);
    void move(unsigned int dt_ms, tmx_map *map);
    int height;
    int width;
    int jumpFrames = 0; // how many frames jump stays active after triggering
    SDL_Rect rect = {(int)x,(int) y, width, height};
     void stopRightMovement();
     void stopUpwardMovement();
     void stopLeftMovement();
     int patrolDir = 0;
} moving_tile;

// Core render functions
void set_color(int color);
void render_map(tmx_map *map);
void draw_all_layers(tmx_map *map, tmx_layer *layers);

// Layer and object drawing
void draw_objects(tmx_object_group *objgr);
void draw_image_layer(tmx_image *image);
void draw_layer(tmx_map *map, tmx_layer *layer);

// Tile and shape drawing
void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags);
void draw_polyline(double **points, double x, double y, int pointsc);
void draw_polygon(double **points, double x, double y, int pointsc);

// Anim and moving tile updates
void update_all_tile_animations(tmx_map *map, unsigned int delta_ms);
void update_tile_anim(tmx_tile *tile, anim_state *state, unsigned int delta_ms);
static inline void draw_moving_tile(tmx_map *map, moving_tile *m);

// Anim states storage for map-used gids
extern anim_state animStates[MAX_TILES];


#endif

