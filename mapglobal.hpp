#ifndef MAPGLOBAL_HPP
#define MAPGLOBAL_HPP

#include <unordered_set>
#include <vector>

#include "Kinematics.hpp"
#include "texture.hpp"
#include "enemy.hpp" // Now relies on enemy.hpp for moving_tile definition

// NOTE: MAX_TILES, anim_state, AIState, and animStates moved to enemy.hpp

// Animation and moving tile updates
void update_all_tile_animations(tmx_map* map, unsigned int delta_ms);
void update_tile_anim(tmx_tile* tile, anim_state* state, unsigned int delta_ms);

// Function to build enemies from map (definition in mapglobal.cpp)
void build_enemies_from_map(tmx_map* map);

// Extern global variables moved to enemy.cpp
extern std::vector<moving_tile> enemies;
extern std::unordered_set<uint64_t> tileSet;

extern tmx_map* map;

#endif

