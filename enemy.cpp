#include "enemy.hpp"
#include "enemyPathing.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib> // For rand()
#include <vector>

// Global patrol points for enemy patrol behavior
const int PATROL_RANGE = 5; // tiles
const std::vector<std::pair<int, int>> patrolPoints = {
    {0, 0}, {PATROL_RANGE, 0}, {PATROL_RANGE, PATROL_RANGE}, {0, PATROL_RANGE}};

// Animate a tile based on delta time
void update_tile_anim(tmx_tile *tile, anim_state *state,
                      unsigned int delta_ms) {
  if (!tile || tile->animation_len == 0 || !state)
    return;
  state->time_acc += delta_ms;
  while (state->time_acc >= tile->animation[state->current_frame].duration) {
    state->time_acc -= tile->animation[state->current_frame].duration;
    state->current_frame = (state->current_frame + 1) % tile->animation_len;
  }
}

// Animate all tiles in the map layer
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

// Update position/velocity from physics
void moving_tile::update() {
  x = physics.kxPos;
  y = physics.kyPos;
  vx = physics.kvelocityX;
  vy = physics.kvelocityY;
}

// Main enemy AI update
void moving_tile::enemyUpdate(float deltaTime, tmx_map *map) {
  physics.kdt = deltaTime;

  // CRITICAL FIX: Disable gravity using the flag
  physics.isGravityOn = false;

  // Set the pushback flag (needed for the physics block below)
  bool isPushBackOn = true;

  float tile_w = static_cast<float>(map->tile_width);
  float tile_h = static_cast<float>(map->tile_height);
  float half_w = width / 2.0f;
  float half_h = height / 2.0f;

  // Calculate Enemy Center Position for AI
  float enemyCenterX = physics.kxPos + half_w;
  float enemyCenterY = physics.kyPos + half_h;

  int tx = static_cast<int>(std::floor(enemyCenterX / tile_w));
  int ty = static_cast<int>(std::floor(enemyCenterY / tile_h));

  // NOTE: Assuming playerX/playerY are external globals defined elsewhere
  int playerTx = static_cast<int>(std::floor(playerX / tile_w));
  int playerTy = static_cast<int>(std::floor(playerY / tile_h));

  // Determine distance to player (tile-based)
  float distToPlayer = std::hypot(playerTx - tx, playerTy - ty);

  // Transition state based on distance
  state = (distToPlayer < 10) ? CHASE : PATROL;

  // Cooldown path update timer
  pathUpdateTimer -= deltaTime;

  // --- Start Movement Calculation ---

  // Set max velocity
  physics.kmaxVel = 1.5f;

  // Save current position for collision rollback if needed
  prevX = x;
  prevY = y;

  const float MAX_STEERING_FORCE = 400.0f;
  float desiredForceX = 0.0f;
  float desiredForceY = 0.0f;

  // --- CONSOLIDATED AI TARGET AND PATHFINDING LOGIC ---
  int goalX, goalY;
  float updateFrequency;

  if (state == CHASE) {
    goalX = playerTx;
    goalY = playerTy;
    updateFrequency = 0.3f;
  } else { // PATROL
    goalX = patrolPoints[patrolStep].first;
    goalY = patrolPoints[patrolStep].second;
    updateFrequency = 1.0f;

    // *** PATROL LOGIC: GENERATE A NEW CHAOTIC PATH SEGMENT ***
    if (currentPath.empty()) {
      // Generate a random 2-tile path segment

      // Random offsets for a 2-step chaotic walk (max 2 tiles away)
      int rx1 = (rand() % 3) - 1; // -1, 0, or 1
      int ry1 = (rand() % 3) - 1;
      int rx2 = (rand() % 3) - 1;
      int ry2 = (rand() % 3) - 1;

      int firstStepX = tx + rx1;
      int firstStepY = ty + ry1;
      int secondStepX = firstStepX + rx2;
      int secondStepY = firstStepY + ry2;

      // Populate path (using current tile as fallback if destination is
      // invalid)
      currentPath.emplace_back(secondStepX, secondStepY);
      currentPath.emplace_back(firstStepX, firstStepY);

      std::reverse(currentPath.begin(),
                   currentPath.end()); // Path from current position to end node

      // Ensure the path always has at least the starting tile center as the
      // target if all else fails
      if (currentPath.empty()) {
        currentPath.emplace_back(tx, ty);
      }
    }
  }

  if (state == CHASE && (pathUpdateTimer <= 0 || currentPath.empty())) {
    currentPath = astarPath(map, tx, ty, goalX, goalY, 1, 1);
    pathUpdateTimer = updateFrequency;
  }

  // --- CONSOLIDATED STEERING LOGIC (used by both CHASE and PATROL) ---
  if (!currentPath.empty()) {
    auto next = currentPath.front();
    // Target the center of the next tile
    float nextX = next.first * tile_w + tile_w / 2.0f;
    float nextY = next.second * tile_h + tile_h / 2.0f;

    // Calculate delta from enemy CENTER to target CENTER
    float deltaX = nextX - enemyCenterX;
    float deltaY = nextY - enemyCenterY;
    float dist = std::hypot(deltaX, deltaY);

    const float ARRIVAL_TOLERANCE = tile_w * 0.3f;

    // Path Advancement logic
    if (dist < ARRIVAL_TOLERANCE) {
      currentPath.erase(currentPath.begin());

      // If path is now empty:
      if (currentPath.empty()) {
        if (state == PATROL) {
          // Patroller reached its destination, advance patrol step
          patrolStep = (patrolStep + 1) % patrolPoints.size();
        }
        goto EndMovementCalculation; // Stop movement this frame
      }

      // Recalculate movement direction for the new node immediately
      next = currentPath.front();
      nextX = next.first * tile_w + tile_w / 2.0f;
      nextY = next.second * tile_h + tile_h / 2.0f;
      deltaX = nextX - enemyCenterX;
      deltaY = nextY - enemyCenterY;
      dist = std::hypot(deltaX, deltaY);
    }

    // Proportional Steering/Slowdown Force
    const float SLOWDOWN_RADIUS = tile_w * 3.0f;

    float forceMagnitude = MAX_STEERING_FORCE;

    if (dist < SLOWDOWN_RADIUS) {
      forceMagnitude = MAX_STEERING_FORCE * (dist / SLOWDOWN_RADIUS);
    }

    // Apply normalized steering force
    if (dist > 0.001f) {
      float normX = deltaX / dist;
      float normY = deltaY / dist;

      // Patrol uses a fixed, slower force with a random kick
      if (state == PATROL) {
        forceMagnitude = MAX_STEERING_FORCE * 0.3f;

        // RANDOM FORCE HACK FOR CHAOTIC MOVEMENT
        float randomWalkForce = 50.0f; // Magnitude of the random kick
        float randX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float randY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

        desiredForceX = normX * forceMagnitude + randX * randomWalkForce;
        desiredForceY = normY * forceMagnitude + randY * randomWalkForce;
      } else {
        // CHASE uses the calculated proportional force
        desiredForceX = normX * forceMagnitude;
        desiredForceY = normY * forceMagnitude;
      }
    }
  }

  // Apply the calculated forces
  physics.applyForce(desiredForceX, desiredForceY);

EndMovementCalculation:;

  // Move using physics
  physics.move();
  update();

  // Collision detection and fix for block penetration
  bool floor, left, right, ceil, overlap;
  SDL_Rect tempRect = {static_cast<int>(x), static_cast<int>(y), width, height};
  checkCollisionsXY(map, floor, left, right, ceil, overlap, tempRect);

  // printf("ENEMY DEBUG: pos=(%.2f,%.2f) kpos=(%.2f,%.2f) kv=(%.2f,%.2f) "
  //        "state=%d pathsz=%zu overlap=%d\n",
  //        x, y, physics.kxPos, physics.kyPos, physics.kvelocityX,
  //        physics.kvelocityY, static_cast<int>(state), currentPath.size(),
  //        overlap);

  // --- DYNAMIC ESCAPE FORCE LOGIC ---
  if (overlap) {
    // 1. Increment counter on collision
    overlap_counter++;

    // 2. Rollback position to last known safe position
    x = prevX;
    y = prevY;
    physics.kxPos = prevX;
    physics.kyPos = prevY;

    // 3. Selective Velocity/Force Killing

    // Horizontal Collision: Kill X velocity/force
    if (left || right) {
      physics.kforceX = 0.0f;
      physics.kvelocityX = 0.0f;
    }

    // Vertical Collision: Kill Y velocity/force
    if (floor || ceil) {
      physics.kforceY = 0.0f;
      physics.kvelocityY = 0.0f;
    }

    // 4. Conditional DYNAMIC Pushback (The Flagged Escape Mechanism)
    if (isPushBackOn && overlap_counter >= 3) {
      // RAMPING ESCAPE FORCE: Base 150.0f + 100.0f per frame past 3 (capped
      // arbitrarily high)
      const float BASE_FORCE = 150.0f;
      const float RAMP_RATE = 100.0f;
      float dynamicEscapeForce = BASE_FORCE + (overlap_counter - 2) * RAMP_RATE;

      // Push enemy away from the blocking wall
      if (left) {
        physics.applyForce(dynamicEscapeForce, 0.0f);

        // STUCK PATROL ADVANCEMENT HACK
        if (state == PATROL) {
          currentPath.clear();
          patrolStep = (patrolStep + 1) % patrolPoints.size();
        }
      } else if (right) {
        physics.applyForce(-dynamicEscapeForce, 0.0f);

        // STUCK PATROL ADVANCEMENT HACK
        if (state == PATROL) {
          currentPath.clear();
          patrolStep = (patrolStep + 1) % patrolPoints.size();
        }
      }

      // Push enemy away from the floor/ceiling
      if (ceil) {
        physics.applyForce(0.0f, dynamicEscapeForce);
      } else if (floor) {
        physics.applyForce(0.0f, -dynamicEscapeForce);
      }
    }

    // 5. Force path recalculation if chasing (only on horizontal block)
    if (state == CHASE && (left || right)) {
      currentPath.clear();
      pathUpdateTimer = 0.0f;
    }
  } else {
    // Reset counter if there was no overlap
    overlap_counter = 0;
  }

  // Update enemy rect for rendering position
  rect.x = static_cast<int>(x);
  rect.y = static_cast<int>(y);

  // Animation update for tiles
  unsigned int dt_ms = static_cast<unsigned int>(deltaTime * 1000.0f);
  update_all_tile_animations(map, dt_ms);
  tmx_tile *current_tile = tmx_get_tile(map, ts_firstgid + base_local_id);
  update_tile_anim(current_tile, &anim, dt_ms);
}

// Stub movement blockers (can be implemented later)
inline void moving_tile::stopRightMovement() {}
inline void moving_tile::stopLeftMovement() {}
inline void moving_tile::stopUpwardMovement() {}
