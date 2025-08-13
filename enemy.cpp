#include "enemy.hpp"
#include "globals.hpp"
#include <SDL2/SDL_render.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <tuple>
// ===================== TILE ANIMATIONS =====================
void update_tile_anim(tmx_tile* tile, anim_state* state, unsigned int delta_ms) {
    if (!tile || tile->animation_len == 0 || !state) return;

    state->time_acc += delta_ms;
    unsigned int frame_duration = tile->animation[state->current_frame].duration;

    if (state->time_acc >= frame_duration) {
        state->time_acc -= frame_duration;
        state->current_frame = (state->current_frame + 1) % tile->animation_len;
        SDL_Log("Tile %d advanced to frame %d", tile->id, state->current_frame);
    }
}

void update_all_tile_animations(tmx_map* map, unsigned int delta_ms) {
    if (!map || !map->ly_head) return;

    tmx_layer* layer = map->ly_head;
    if (layer->type != L_LAYER || !layer->visible) return;

    unsigned int total = map->width * map->height;
    for (unsigned int i = 0; i < total; ++i) {
        unsigned int gid = layer->content.gids[i] & TMX_FLIP_BITS_REMOVAL;
        if (gid == 0 || gid >= MAX_TILES) continue;

        tmx_tile* tile = tmx_get_tile(map, gid);
        if (tile && tile->animation_len > 0) {
            anim_state* state = &animStates[gid];
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

bool isWalkable(tmx_map* map, int tx, int ty) {
    SDL_Rect temp = {
        static_cast<int>(tx * map->tile_width),
        static_cast<int>(ty * map->tile_height),
        static_cast<int>(map->tile_width),
        static_cast<int>(map->tile_height)
    };
    bool floor, left, right, ceil, overlap;
    checkCollisionsXY(map, floor, left, right, ceil, overlap, temp);
    return floor;
}

std::vector<std::pair<int,int>> dijkstraPath(tmx_map* map,
                                             int startX, int startY,
                                             int goalX, int goalY) {
    int w = map->width;
    int h = map->height;

auto inBounds = [&](int tx, int ty) {
    return tx >= 0 && tx < w && ty >= 0 && ty < h;
};

if (!inBounds(startX, startY) || !inBounds(goalX, goalY)) {
    SDL_Log("Pathfinding aborted: start/goal out of map bounds");
    return {};
}
    std::vector<std::vector<float>> dist(w, std::vector<float>(h, std::numeric_limits<float>::max()));
    std::vector<std::vector<std::pair<int,int>>> prev(w, std::vector<std::pair<int,int>>(h, {-1,-1}));
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    dist[startX][startY] = 0;
    pq.push({startX, startY, 0});

    int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}}; // right, left, down, up

    while(!pq.empty()) {
        Node node = pq.top(); pq.pop();
        if (node.x == goalX && node.y == goalY) break;

        for(auto &d: dirs){
            int nx = node.x + d[0];
            int ny = node.y + d[1];
            if(nx<0 || nx>=w || ny<0 || ny>=h) continue;
            if(!isWalkable(map, nx, ny)) continue;

            float newCost = dist[node.x][node.y] + 1.0f;
            if(newCost < dist[nx][ny]){
                dist[nx][ny] = newCost;
                prev[nx][ny] = {node.x, node.y};
                pq.push({nx, ny, newCost});
            }
        }
    }

    std::vector<std::pair<int,int>> path;
int cx = goalX, cy = goalY;

// If no path, prev remains {-1,-1}
if (!inBounds(cx, cy) || prev[cx][cy].first == -1) {
    SDL_Log("Pathfinding: no path found");
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

    // Reset collision flags
    physics.passThisFrameYPos = false;
    physics.passThisFrameYNeg = false;
    physics.passThisFramePosX = false;
    physics.passThisFrameNegX = false;
}

void moving_tile::enemyUpdate(float deltaTime, tmx_map* map) {
    unsigned int dt_ms = static_cast<unsigned int>(deltaTime * 1000.0f);

    // Update static map animations
    update_all_tile_animations(map, dt_ms);

    // Enemy tile animation
    update_tile_anim(
        tmx_get_tile(map, ts_firstgid + base_local_id),
        &anim,
        dt_ms
    );

    // Move enemy with pathfinding & physics
    move(dt_ms, map);

    // Sync physics for rendering/logging
    update();
}

// ===================== ENEMY MOVEMENT =====================

void moving_tile::move(unsigned int dt_ms, tmx_map* map) {
    physics.kdt = dt_ms / 1000.0f;
    rect.w = 16;
    rect.h = 16;
    bool wantJump = false;
    // Check wall collision BEFORE deciding movement
    bool onGround=false, wallLeft=false, wallRight=false, onCeiling=false, overlapping=false;
    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);
    checkCollisionsXY(map, onGround, wallLeft, wallRight, onCeiling, overlapping, rect);

    // If stuck against left OR right wall, freeze horizontal movement entirely
    if (wallLeft) {
    if (physics.kvelocityX < 0) physics.kvelocityX = 0;
    physics.kaccelerationX = 0;
    physics.kforceX = 0;
}

if (wallRight) {
    if (physics.kvelocityX > 0) physics.kvelocityX = 0;
    physics.kaccelerationX = 0;
    physics.kforceX = 0;
}
  // ===== PATHFINDING LOGIC =====
        int startX = static_cast<int>(std::round(x / map->tile_width));
        int startY = static_cast<int>(std::round(y / map->tile_height));
        int goalX = static_cast<int>(std::round(playerX / map->tile_width));
        int goalY = static_cast<int>(std::round(playerY / map->tile_height));
        std::vector<std::pair<int,int>> path = dijkstraPath(map, startX, startY, goalX, goalY);

        bool moveLeft=false, moveRight=false, jump=false;
        float threshold = map->tile_width / 2.0f;
        if(!path.empty()) {
            auto next = path.front();
            float nextX = next.first * map->tile_width;
            float nextY = next.second * map->tile_height;
            if(nextX > x + threshold) moveRight = true;
            else if(nextX < x - threshold) moveLeft = true;
            float jumpHeightThreshold = map->tile_height * 0.5f;
            if ((y - nextY) > jumpHeightThreshold) {
                jump = true;
                jumpFrames = 10;
            }
        } else {
            // Patrol Mode
            if(patrolDir==0) patrolDir=1;
            if(patrolDir>0) moveRight=true;
            else moveLeft=true;
            // Flip patrol dir on wall
            bool dummyGround, wallL, wallR, ceilingC, overlapC;
            rect.x = static_cast<int>(x);
            rect.y = static_cast<int>(y);
            checkCollisionsXY(map, dummyGround, wallL, wallR, ceilingC, overlapC, rect);
            if(wallR) patrolDir=-1;
            if(wallL) patrolDir=1;
        }
        if(wantJump)
{
	jump = true;
}
        // Apply Forces
        if(moveLeft) physics.applyForce(-100,0);
        if(moveRight) physics.applyForce(100,0);
if(jumpFrames > 0 && onGround) {physics.applyForce(0,-50000);
            jumpFrames--;
	}
    

    // Gravity
    if(!onGround) physics.applyForce(0, 9800);
    else{
       physics.kvelocityY = 0;
    }
    // Commit to physics
    physics.kmaxVel = 0.5f;
    physics.move();
    cout << "wallLeft: " << wallLeft << " wallRight: " << wallRight << endl;
    cout << "jumpFrames: " << jumpFrames << endl;
    cout << "jump: " <<jump << endl;
    if(wantJump)
{
	wantJump = false;
}
    if(wallLeft || wallRight)
    {
        cout << "Here!!! " << endl;	
	wantJump = true;
	jumpFrames = 2;
    }
    // Sync rect
    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);
}
// ===================== HELPERS =====================
inline void moving_tile::stopRightMovement() {
    if (physics.kvelocityX > 0) {
        physics.kvelocityX = 0;
        physics.kaccelerationX = 0;
        physics.kforceX = 0;
        physics.passThisFramePosX = true;
    }
}

inline void moving_tile::stopLeftMovement() {
    if (physics.kvelocityX < 0) {
        physics.kvelocityX = 0;
        physics.kaccelerationX = 0;
        physics.kforceX = 0;
        physics.passThisFrameNegX = true;
    }
}

inline void moving_tile::stopUpwardMovement() {
    if (physics.kvelocityY < 0) {
        physics.kvelocityY = 0;
        physics.passThisFrameYNeg = true;
    }
}
