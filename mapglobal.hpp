
#ifndef MAPGLOBAL_HPP
#define MAPGLOBAL_HPP
#include <unordered_set>
#include <SDL2/SDL.h>
#include <tmx.h>
#include "Kinematics.hpp"
#include "texture.hpp"
const int MAX_TILES = 1000;
typedef struct {
    int current_frame;
    unsigned int time_acc;
} anim_state;
// Anim and moving tile updates
void update_all_tile_animations(tmx_map *map, unsigned int delta_ms);
void update_tile_anim(tmx_tile *tile, anim_state *state, unsigned int delta_ms);

// Anim states storage for map-used gids
extern anim_state animStates[MAX_TILES];


// A lightweight movable entity that uses a tileset tile/animation for visuals
typedef struct {
    float x, y;                // world position in pixels
    float vx, vy;              // velocity in px/s
    anim_state anim;           // animation state
    tmx_tileset *ts;           // owning tileset pointer
    unsigned int ts_firstgid;  // tileset’s firstgid (from tmx_tileset_list)
    unsigned int base_local_id;// base local tile id in the tileset (the tile holding animation)		       
    unsigned int id;
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
     int health = 10;
     int atk = 2;
     double cooldown = 1;
     WTexture texture;
     double accDt;
  double mana;
} moving_tile;

void build_enemies_from_map(tmx_map *map);
extern std::vector<moving_tile> enemies;
extern std::unordered_set<uint64_t> tileSet;
extern tmx_map *map;

#endif

