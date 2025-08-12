#include"enemy.hpp"



void update_tile_anim(tmx_tile *tile, anim_state *state, unsigned int delta_ms) {
    if (!tile || tile->animation_len == 0 || !state) return;
    state->time_acc += delta_ms;
    unsigned int frame_duration = tile->animation[state->current_frame].duration;
    if (state->time_acc >= frame_duration) {
        state->time_acc -= frame_duration;
        state->current_frame = (state->current_frame + 1) % tile->animation_len;
        SDL_Log("Tile %d advanced to frame %d", tile->id, state->current_frame);
    }
}

void update_all_tile_animations(tmx_map *map, unsigned int delta_ms) {
    // Iterate only the first tile layer’s gids array (as your code assumes)
    if (!map || !map->ly_head) return;
    tmx_layer *layer = map->ly_head;
    if (layer->type != L_LAYER || !layer->visible) return;

    unsigned int total = map->width * map->height;
    for (unsigned int i = 0; i < total; ++i) {
        unsigned int gid = layer->content.gids[i] & TMX_FLIP_BITS_REMOVAL;
        if (gid == 0) continue;
        if (gid >= MAX_TILES) continue;
        tmx_tile *tile = tmx_get_tile(map, gid);
        if (tile && tile->animation_len > 0) {
            anim_state *state = &animStates[gid];
            update_tile_anim(tile, state, delta_ms);
        }
    }
}

void moving_tile::enemyUpdate(float deltaTime, tmx_map *map)
{
    update_all_tile_animations(map, (unsigned int)(deltaTime * 1000.0f));
    update_tile_anim(tmx_get_tile(map, ts_firstgid + base_local_id), &anim, (unsigned int)(deltaTime * 1000.0f));

    move((unsigned int)(deltaTime * 1000.0f), map); // Physics step
    update(); // Sync for rendering
}
void moving_tile::update()
{
		std::cout << physics.kvelocityY << " " << vy <<  std::endl;
 std::cout << "force y: " << physics.kforceY <<std::endl;
  vx = physics.kvelocityX;
  vy = physics.kvelocityY;
  x = physics.kxPos;
y = physics.kyPos;
}


void moving_tile::move(unsigned int dt_ms, tmx_map *map)
{
    bool moveLeft, moveRight, jump;
    // Set delta time in physics for this frame
    physics.kdt = dt_ms / 1000.0; // seconds

    // Update physics
    physics.move();

    // Sync back into moving_tile for rendering
    x = physics.kxPos;
    y = physics.kyPos;
    rect.x = x;
    rect.y = y;
    rect.w = 16;
    rect.h = 16;
    float diffX = playerX - x;
    float diffY = playerY - y;
    if (diffX > 0) {	
	moveLeft = false;
	moveRight = true;
    }
    else if(diffX < 0)
    {
	moveRight = false;
	moveLeft = true;
    }
    else{
	physics.kvelocityX = 0;
    }

    cout << "x: " << rect.x << "y: " << rect.y << "w: " << rect.w << "h: " << rect.h << endl;
    bool onGround = false, wallLeft = false, wallRight = false, onCeiling = false, overlapping = false;
    checkCollisionsXY(map, onGround, wallLeft, wallRight, onCeiling, overlapping, rect);
    std::cout << "onGround: " << onGround << std::endl;
    // ===== GRAVITY =====
    if (!onGround) {
        physics.applyForce(0, (60 * physics.kgravityConstant * physics.kdt)); // pull down
    } else {
        if (physics.kvelocityY > 0) { // only stop downward movement
            physics.kvelocityY = 0;
            physics.passThisFrameYPos = true; 
        }
    }

    // ===== INPUT FORCES =====
    if (moveLeft) {
        physics.applyForce(-100, 0);
    }
    if (moveRight) {
        physics.applyForce(100, 0);
    }
    if (jump && onGround) {
        physics.applyForce(0, -130000 * physics.kdt); // instant upward push
    }

    // ===== FRICTION =====
    if (!(moveLeft || moveRight)) {
        if (std::abs(physics.kvelocityX) > 0.3) {
            physics.kvelocityX *= 0.85;
        } else {
            physics.kvelocityX = 0;
        }
    }
    else{
	    jump = true;
    }

    // ===== PREVENT TUNNELING =====
    if (wallRight) {
        physics.kvelocityX     = physics.kvelocityX > 0 ? 0 : physics.kvelocityX;
physics.kaccelerationX = physics.kaccelerationX > 0 ? 0 : physics.kaccelerationX;
        physics.kforceX        = physics.kforceX > 0 ? 0 : physics.kforceX;
        physics.passThisFramePosX = true;
    }
    if (wallLeft) {
        physics.kvelocityX     = physics.kvelocityX < 0 ? 0 : physics.kvelocityX;
        physics.kaccelerationX = physics.kaccelerationX < 0 ? 0 : physics.kaccelerationX;
        physics.kforceX        = physics.kforceX < 0 ? 0 : physics.kforceX;
        physics.passThisFrameNegX = true;
    }
    if (onCeiling) {
        if (physics.kvelocityY < 0) { // only stop upward motion
            physics.kvelocityY = 0;
            physics.passThisFrameYNeg = true;
        }
    }

 
}
