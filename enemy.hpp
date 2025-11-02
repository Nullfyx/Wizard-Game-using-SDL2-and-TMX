#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "globals.hpp"
#include "collisions.hpp"
#include "tmx.h"
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_set>
#include <cmath>
#include "Kinematics.hpp"
#include "texture.hpp"

// ===================== CORE GLOBAL TYPES & CONSTANTS =====================
// This is the new structural location for these definitions.
const int MAX_TILES = 8999;

typedef struct {
    int current_frame;
    unsigned int time_acc;
} anim_state;

enum AIState { PATROL, CHASE };

// Anim states storage
inline anim_state animStates[MAX_TILES] = {};


// ===================== MOVING TILE CLASS (FULL DEFINITION) =====================
typedef struct {
    float x, y;
    float vx, vy;
    anim_state anim;
    tmx_tileset *ts;
    unsigned int ts_firstgid;
    unsigned int base_local_id;
    unsigned int id;
    Kinematics physics;
    WTexture texture;

    // --- Enemy Properties ---
    int height = 16;
    int width = 16;
    int jumpFrames = 0;
    int patrolDir = 0;
    int health = 10;
    int atk = 2;
    double cooldown = 1;
    double accDt;
    double mana;

    // --- AI/Movement Cache ---
    AIState state = PATROL;
    std::vector<std::pair<int, int>> currentPath;
    float pathUpdateTimer = 0.0f;
    int patrolStep = 0;
    
    // *** NEW: Consecutive overlap counter for dynamic escape force ***
    int overlap_counter = 0;

    // --- Physics/Rendering Rect ---
    SDL_Rect rect = {(int)x, (int)y, width, height};

    // --- NEW: Previous position for collision resolution ---
    float prevX = 0.0f;
    float prevY = 0.0f;

    // --- Methods (Definitions in enemy.cpp) ---
    void update();
    void enemyUpdate(float deltaTime, tmx_map* map);
    void move(unsigned int dt_ms, tmx_map *map);
    void stopRightMovement();
    void stopUpwardMovement();
    void stopLeftMovement();

} moving_tile;


// ===================== ENEMY GLOBAL STORAGE (Declarations) =====================
extern std::vector<moving_tile> enemies;
extern std::unordered_set<uint64_t> tileSet;

#endif
