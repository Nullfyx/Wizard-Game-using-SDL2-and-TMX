
#include "projectile.hpp"
#include "collisions.hpp"
#include "globals.hpp"
#include "mapglobal.hpp"
#include <cmath>
#include <climits>

int projectile::index = 0;


projectile::projectile(int startX, int startY, int targetX, int targetY) {
    cout << "start: " << startX << "  " << startY << endl;
    cout << "end: " << targetX << "  " << targetY << endl;

    texture.WIMG_Load("sprites/bolt.png");
    physics.kvelocityX = 0.0f;
    physics.kvelocityY = 0.0f;
    destroyMe = false;
    index++;

    // store world coordinates in physics
    physics.kxPos = (float)startX;
    physics.kyPos = (float)startY;

    // set velocity towards the target (world space)
    float dx = targetX - startX;
    float dy = targetY - startY;
    float dist = std::sqrt(dx*dx + dy*dy);
angle = std::atan2(-dy, dx) * 180.0f / M_PI;
    if (dist != 0) {
        float speed = 2.0f;
        physics.kvelocityX = (dx / dist) * speed;
        physics.kvelocityY = (dy / dist) * speed; // no need to flip
    }

    // initialize proRect relative to camera for rendering only
    proRect = { startX - camera.x, startY - camera.y, 6, 1 };
}

void projectile::update() {
    // update world position
    physics.move();

    // update proRect for rendering only
    proRect.x = (int)(physics.kxPos - camera.x);
    proRect.y = (int)(physics.kyPos - camera.y);
    if(angle < 0) angle += 360.0f;  
    texture.render(wRenderer, nullptr, &proRect,  -angle);

    // future rect for collision
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
