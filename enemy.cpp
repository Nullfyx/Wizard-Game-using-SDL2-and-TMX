#include "enemy.hpp"
#include "globals.hpp"
#include <SDL2/SDL_render.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <vector>
// ===================== TILE ANIMATIONS =====================
void update_tile_anim(tmx_tile *tile, anim_state *state,
                      unsigned int delta_ms) {
  if (!tile || tile->animation_len == 0 || !state)
    return;

  state->time_acc += delta_ms;
  unsigned int frame_duration = tile->animation[state->current_frame].duration;

  if (state->time_acc >= frame_duration) {
    state->time_acc -= frame_duration;
    state->current_frame = (state->current_frame + 1) % tile->animation_len;
  }
}

void update_all_tile_animations(tmx_map *map, unsigned int delta_ms) {
  if (!map || !map->ly_head)
    return;

  tmx_layer *layer = map->ly_head;
  if (layer->type != L_LAYER || !layer->visible)
    return;

  unsigned int total = map->width * map->height;
  for (unsigned int i = 0; i < total; ++i) {
    unsigned int gid = layer->content.gids[i] & TMX_FLIP_BITS_REMOVAL;
    if (gid == 0 || gid >= MAX_TILES)
      continue;

    tmx_tile *tile = tmx_get_tile(map, gid);
    if (tile && tile->animation_len > 0) {
      anim_state *state = &animStates[gid];
      update_tile_anim(tile, state, delta_ms);
    }
  }
}

// ===================== PATHFINDING SUPPORT =====================
struct Node {
  int x, y;
  float cost;
  bool operator>(const Node &other) const { return cost > other.cost; }
};

bool isWalkable(tmx_map *map, int tx, int ty) {
  SDL_Rect temp = {static_cast<int>(tx * map->tile_width),
                   static_cast<int>(ty * map->tile_height),
                   static_cast<int>(map->tile_width),
                   static_cast<int>(map->tile_height)};
  bool floor, left, right, ceil, overlap;
  checkCollisionsXY(map, floor, left, right, ceil, overlap, temp);
  return floor;
}

std::vector<std::pair<int, int>>
dijkstraPath(tmx_map *map, int startX, int startY, int goalX, int goalY) {
  int w = map->width;
  int h = map->height;

  auto inBounds = [&](int tx, int ty) {
    return tx >= 0 && tx < w && ty >= 0 && ty < h;
  };

  if (!inBounds(startX, startY) || !inBounds(goalX, goalY)) {
    SDL_Log("Pathfinding aborted: start/goal out of map bounds");
    return {};
  }
  std::vector<std::vector<float>> dist(
      w, std::vector<float>(h, std::numeric_limits<float>::max()));
  std::vector<std::vector<std::pair<int, int>>> prev(
      w, std::vector<std::pair<int, int>>(h, {-1, -1}));
  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

  dist[startX][startY] = 0;
  pq.push({startX, startY, 0});

  int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; // right, left, down, up

  while (!pq.empty()) {
    Node node = pq.top();
    pq.pop();
    if (node.x == goalX && node.y == goalY)
      break;

    for (auto &d : dirs) {
      int nx = node.x + d[0];
      int ny = node.y + d[1];
      if (nx < 0 || nx >= w || ny < 0 || ny >= h)
        continue;
      if (!isWalkable(map, nx, ny))
        continue;

      float newCost = dist[node.x][node.y] + 1.0f;
      if (newCost < dist[nx][ny]) {
        dist[nx][ny] = newCost;
        prev[nx][ny] = {node.x, node.y};
        pq.push({nx, ny, newCost});
      }
    }
  }

  std::vector<std::pair<int, int>> path;
  int cx = goalX, cy = goalY;

  // If no path, prev remains {-1,-1}
  if (!inBounds(cx, cy) || prev[cx][cy].first == -1) {
    return {};
  }

  while (inBounds(cx, cy) && prev[cx][cy].first != -1) {
    path.push_back({cx, cy});
    auto p = prev[cx][cy];
    cx = p.first;
    cy = p.second;
  }

  std::reverse(path.begin(), path.end());
  return path;
}

// ===================== ENEMY UPDATE =====================
void moving_tile::update() {
  vx = physics.kvelocityX;
  vy = physics.kvelocityY;
  x = physics.kxPos;
  y = physics.kyPos;
}

void moving_tile::enemyUpdate(float deltaTime, tmx_map *map) {
  unsigned int dt_ms = static_cast<unsigned int>(deltaTime * 1000.0f);

  // Update static map animations
  update_all_tile_animations(map, dt_ms);

  // Enemy tile animation
  update_tile_anim(tmx_get_tile(map, ts_firstgid + base_local_id), &anim,
                   dt_ms);

  // Move enemy with pathfinding & physics
  move(dt_ms, map);
}

// ===================== ENEMY MOVEMENT =====================
void moving_tile::move(unsigned int dt_ms, tmx_map *map) {
  // Reset collision flags
  physics.passThisFrameYPos = false;
  physics.passThisFrameYNeg = false;
  physics.passThisFramePosX = false;
  physics.passThisFrameNegX = false;

  float dt_sec = dt_ms / 1000.0f;
  physics.kdt = dt_sec;

  rect.x = static_cast<int>(x);
  rect.y = static_cast<int>(y);

  // ===== COLLISION CHECK =====
  bool onGround = false, wallLeft = false, wallRight = false, onCeiling = false,
       overlapping = false;
  checkCollisionsXY(map, onGround, wallLeft, wallRight, onCeiling, overlapping,
                    rect);

  // ===== PATHFINDING & PATROL =====
  int startX = static_cast<int>(std::round(x / map->tile_width));
  int startY = static_cast<int>(std::round(y / map->tile_height));
  int goalX = static_cast<int>(std::round(playerX / map->tile_width));
  int goalY = static_cast<int>(std::round(playerY / map->tile_height));

  std::vector<std::pair<int, int>> path =
      dijkstraPath(map, startX, startY, goalX, goalY);

  bool moveLeft = false, moveRight = false, wantJump = false;
  float threshold = map->tile_width / 2.0f;

  if (!path.empty()) {
    auto next = path.front();
    float nextX = next.first * map->tile_width;
    float nextY = next.second * map->tile_height;

    if (nextX > x + threshold)
      moveRight = true;
    else if (nextX < x - threshold)
      moveLeft = true;

    if ((y - nextY) > map->tile_height * 0.5f)
      wantJump = true;
  } else {
    if (patrolDir == 0)
      patrolDir = 1;
    if (patrolDir > 0)
      moveRight = true;
    else
      moveLeft = true;
    if (wallRight)
      patrolDir = -1;
    if (wallLeft)
      patrolDir = 1;
  }
// ===== CLIFF / LEDGE DETECTION =====
if (onGround) {
  int aheadX = static_cast<int>((x + (moveRight ? map->tile_width : -map->tile_width)) / map->tile_width);
  int aheadY = static_cast<int>((y + map->tile_height) / map->tile_height);

  bool groundAhead = isWalkable(map, aheadX, aheadY);

  // Only care about ledge if player is NOT below
  bool playerBelow = (playerY > y);

  if (!groundAhead && !playerBelow) {
    if (moveRight) {
      moveRight = false;
      moveLeft = true;
      patrolDir = -1;
    } else if (moveLeft) {
      moveLeft = false;
      moveRight = true;
      patrolDir = 1;
    }
  }
}

  // ===== APPLY FORCES =====
  if (moveLeft)
    physics.applyForce(-200, 0);
  if (moveRight)
    physics.applyForce(200, 0);
  if (!onGround)
    physics.applyForce(0, 9800);
  else {
    physics.kvelocityY = 0;
    y = std::floor(y / map->tile_height) * map->tile_height;
  }

  if (wantJump && onGround) {
    physics.applyForce(0, -50000);
    jumpFrames = 10;
  }

  // ===== WALL COLLISION CORRECTION =====
  if (wallLeft)
    stopLeftMovement();
  if (wallRight)
    stopRightMovement();

  // Overlap correction
  if (overlapping) {
    if (wallLeft && physics.kvelocityX < 0)
      physics.kvelocityX = 0;
    if (wallRight && physics.kvelocityX > 0)
      physics.kvelocityX = 0;
  }

  // Clamp tiny velocities
  if (std::abs(physics.kvelocityX) < 0.05f)
    physics.kvelocityX = 0;
  if (std::abs(physics.kvelocityY) < 0.05f)
    physics.kvelocityY = 0;

  // ===== COMMIT PHYSICS =====
  physics.kmaxVel = 0.5f;
  physics.move();
  // Sync physics for rendering/logging
  update();

  rect.x = static_cast<int>(x);
  rect.y = static_cast<int>(y);
}

// ===================== HELPERS =====================
inline void moving_tile::stopRightMovement() {
  if (physics.kvelocityX > 0 || vy > 0) {
    physics.kvelocityX = 0;
    physics.kaccelerationX = 0;
    physics.kforceX = 0;
    vy = 0;
    physics.passThisFramePosX = true;
  }
}

inline void moving_tile::stopLeftMovement() {
  if (physics.kvelocityX < 0 || vx < 0) {
    physics.kvelocityX = 0;
    physics.kaccelerationX = 0;
    physics.kforceX = 0;
    vx = 0;
    physics.passThisFrameNegX = true;
  }
}

inline void moving_tile::stopUpwardMovement() {
  if (physics.kvelocityY < 0) {
    physics.kvelocityY = 0;
    physics.passThisFrameYNeg = true;
  }
}
