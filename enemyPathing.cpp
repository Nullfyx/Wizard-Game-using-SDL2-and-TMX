#include "enemyPathing.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <vector>

// Helper: Check map boundaries
inline bool inBounds(tmx_map *map, int tx, int ty) {
  return tx >= 0 && ty >= 0 && tx < map->width && ty < map->height;
}

// Helper: Check if all tiles covered by enemy bounding box at (tx, ty) are free
bool isPassableForEnemy(tmx_map *map, int tx, int ty, int enemyWidthTiles,
                        int enemyHeightTiles) {
  for (int dx = 0; dx < enemyWidthTiles; ++dx) {
    for (int dy = 0; dy < enemyHeightTiles; ++dy) {
      int checkX = tx + dx;
      int checkY = ty + dy;
      if (!inBounds(map, checkX, checkY))
        return false;
      if (main_layer_gid_at(map, checkX, checkY) != 0)
        return false; // solid tile blocks
    }
  }
  return true;
}

// Heuristic: Manhattan distance
inline float heuristic(int x, int y, int gx, int gy) {
  return std::abs(gx - x) + std::abs(gy - y);
}
tmx_layer *get_main_tile_layer(tmx_map *map) {
  for (tmx_layer *layer = map->ly_head; layer; layer = layer->next) {
    if (layer->type == L_LAYER && layer->visible) {
      return layer;
    }
  }
  return nullptr;
}

unsigned int main_layer_gid_at(tmx_map *map, int tx, int ty) {
  tmx_layer *layer = get_main_tile_layer(map);
  if (!layer)
    return 0;
  if (tx < 0 || tx >= (int)map->width || ty < 0 || ty >= (int)map->height)
    return 0;
  int index = ty * map->width + tx;
  return layer->content.gids[index];
}

// Main A* with dynamic reroute attempts for blocking tiles
std::vector<std::pair<int, int>> astarPath(tmx_map *map, int startX, int startY,
                                           int goalX, int goalY,
                                           int enemyWidthTiles,
                                           int enemyHeightTiles) {
  if (!map)
    return {};
  if (!inBounds(map, startX, startY) || !inBounds(map, goalX, goalY))
    return {};

  struct Node {
    int x, y;
    float cost, heuristicValue;
    bool operator>(const Node &rhs) const {
      return (cost + heuristicValue) > (rhs.cost + rhs.heuristicValue);
    }
  };

  int w = map->width;
  int h = map->height;

  auto inBoundsLambda = [&](int x, int y) {
    return x >= 0 && y >= 0 && x < w && y < h;
  };

  // Cost to reach each tile, infinite init
  std::vector<std::vector<float>> dist(
      w, std::vector<float>(h, std::numeric_limits<float>::max()));
  std::vector<std::vector<std::pair<int, int>>> prev(
      w, std::vector<std::pair<int, int>>(h, {-1, -1}));

  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

  dist[startX][startY] = 0;
  pq.push({startX, startY, 0, heuristic(startX, startY, goalX, goalY)});

  // Directions for main 4 neighbors + 8 extended neighbors for reroute attempts
  const std::vector<std::pair<int, int>> directions = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

  while (!pq.empty()) {
    Node node = pq.top();
    pq.pop();
    if (node.x == goalX && node.y == goalY)
      break;

    if (node.cost > dist[node.x][node.y])
      continue;

    for (auto &d : directions) {
      int nx = node.x + d.first;
      int ny = node.y + d.second;
      if (!inBoundsLambda(nx, ny))
        continue;

      // Check if tile plus enemy bounding box is passable
      if (!isPassableForEnemy(map, nx, ny, enemyWidthTiles, enemyHeightTiles)) {
        // Attempt reroute: try adjacent tiles around this blocking tile
        // dynamically Try shifts orthogonal to attempted movement
        std::vector<std::pair<int, int>> rerouteOffsets = {
            {d.first, 0}, {0, d.second}, {-d.first, 0}, {0, -d.second}};
        bool rerouteFound = false;
        for (auto &ro : rerouteOffsets) {
          int rnx = node.x + ro.first;
          int rny = node.y + ro.second;
          if (inBoundsLambda(rnx, rny) &&
              isPassableForEnemy(map, rnx, rny, enemyWidthTiles,
                                 enemyHeightTiles)) {
            float newCost =
                dist[node.x][node.y] + 1.5f; // slightly costlier for reroute
            if (newCost < dist[rnx][rny]) {
              dist[rnx][rny] = newCost;
              prev[rnx][rny] = {node.x, node.y};
              pq.push({rnx, rny, newCost, heuristic(rnx, rny, goalX, goalY)});
              rerouteFound = true;
              break;
            }
          }
        }
        if (rerouteFound)
          continue; // skip normal push for blocked tile
        else
          continue; // blocked tile no reroute possible
      }

      // Normal passable move
      float newCost =
          dist[node.x][node.y] +
          ((d.first != 0 && d.second != 0) ? 1.4f : 1.0f); // diagonal cost ~1.4
      if (newCost < dist[nx][ny]) {
        dist[nx][ny] = newCost;
        prev[nx][ny] = {node.x, node.y};
        pq.push({nx, ny, newCost, heuristic(nx, ny, goalX, goalY)});
      }
    }
  }

  // Reconstruct path
  std::vector<std::pair<int, int>> path;
  int cx = goalX, cy = goalY;
  if (prev[cx][cy].first == -1 && !(cx == startX && cy == startY))
    return {}; // no path

  while (!(cx == startX && cy == startY)) {
    path.emplace_back(cx, cy);
    auto p = prev[cx][cy];
    cx = p.first;
    cy = p.second;
    if (cx == -1)
      break;
  }
  path.emplace_back(startX, startY);
  std::reverse(path.begin(), path.end());
  return path;
}
