
#include "projectile.hpp"
#include "collisions.hpp"
#include "globals.hpp"
#include "mapglobal.hpp"
#include <cmath>
#include <climits>
#include <iostream>

int projectile::index = 0;

projectile::projectile(int startX, int startY, int targetX, int targetY) {
    std::cout << "start: " << startX << "  " << startY << std::endl;
    std::cout << "end: " << targetX << "  " << targetY << std::endl;

    texture.WIMG_Load("sprites/bolt.png");
    physics.kvelocityX = 0.0f;
    physics.kvelocityY = 0.0f;
    destroyMe = false;
    index++;

    // store world coordinates
    physics.kxPos = (float)startX;
    physics.kyPos = (float)startY;

    // compute velocity
    float dx = targetX - startX;
    float dy = targetY - startY;
    float dist = std::sqrt(dx*dx + dy*dy);

    angle = std::atan2(-dy, dx) * 180.0f / M_PI;

    if (dist != 0) {
        float speed = 2.0f;
        physics.kvelocityX = (dx / dist) * speed;
        physics.kvelocityY = (dy / dist) * speed;
    }

    // initial rect
    proRect = { startX - camera.rect.x, startY - camera.rect.y, 6, 1 };

    // start cooldown timer
    timer.start();
}

void projectile::update() {
    physics.move();

    proRect.x = (int)(physics.kxPos - camera.rect.x);
    proRect.y = (int)(physics.kyPos - camera.rect.y);

    if(angle < 0) angle += 360.0f;  
    texture.render(wRenderer, nullptr, &proRect, -angle);

    SDL_Rect futureRect = { (int)(physics.kxPos + physics.kvelocityX),
                            (int)(physics.kyPos + physics.kvelocityY),
                            proRect.w, proRect.h };

    bool floorCollision = false, leftWallCollision = false, rightWallCollision = false,
         ceilingCollision = false, overlapping = false;

    checkCollisionsXY(map, floorCollision, leftWallCollision, rightWallCollision,
                      ceilingCollision, overlapping, futureRect);

    if (overlapping || floorCollision || leftWallCollision || rightWallCollision || ceilingCollision) {
        destroyMe = true;
    }
}

// return true if enough time passed since last fire
bool projectile::canFire() {
    Uint32 elapsedMs = timer.getTicks();
    cout << elapsedMs << endl;
    if (elapsedMs >= (Uint32)(cooldown * 1000)) {
        timer.start(); // reset cooldown
        return true;
    }
    return false;
}
