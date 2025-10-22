#ifndef ENEMYPATHING_HPP
#define ENEMYPATHING_HPP
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include "tmx.h"
#include "enemy.hpp" // Includes necessary AIState definition


// ===================== A* CORE TYPES =====================

struct Node {
  int x, y;
  float cost, heuristic;
  bool operator>(const Node &rhs) const;
};

// ===================== TILE CHECK HELPERS =====================
// These are external linkage functions defined in enemyPathing.cpp
tmx_layer* get_main_tile_layer(tmx_map *map);
unsigned int main_layer_gid_at(tmx_map *map, int tx, int ty);
bool is_solid_tile(tmx_map *map, int tx, int ty);
bool is_valid_standing_tile(tmx_map *map, int tx, int ty);
float heuristic(int x, int y, int gx, int gy);

// ===================== MAIN PATHING FUNCTION =====================
std::vector<std::pair<int, int>> astarPath(tmx_map *map, int startX, int startY, int goalX, int goalY, int enemyWidthTiles = 1, int enemyHeightTiles = 1);

#endif
