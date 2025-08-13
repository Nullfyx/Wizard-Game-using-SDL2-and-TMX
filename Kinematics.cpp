
#include "Kinematics.hpp"

Kinematics::Kinematics()
{
    kgravityConstant = 9.8;
    kmass = 40;
    kxPos = 0;
    kyPos = 0;
    kvelocityY = 0;
    kvelocityX = 0;
    kaccelerationX = 0;
    kaccelerationY = 0;
    kforceX = 0;
    kforceY = 0;
    kaccelerationY = kgravityConstant;
    positions = {0, 0};
    passThisFrameNegX = false;
    passThisFrameYPos = false;
    passThisFramePosX = false;
    passThisFrameYNeg = false;
    kmaxVel = 3;
}

void Kinematics::applyForce(double fX, double fY)
{
    kforceX += fX;
    kforceY += fY;
}

void Kinematics::move()
{
    // Acceleration
    kaccelerationX = kforceX / kmass;
    kaccelerationY = kforceY / kmass;

    // Velocity update
    kvelocityX += kaccelerationX * kdt;
    kvelocityY += kaccelerationY * kdt;

    playerVelX = kvelocityX;
    playerVelY = kvelocityY;

    // Cap X velocity
    if (kvelocityX > kmaxVel)  kvelocityX = kmaxVel;
    if (kvelocityX < -kmaxVel) kvelocityX = -kmaxVel;

    // --- Y movement ---
    if (!passThisFrameYPos && !passThisFrameYNeg) {
        // No collision — normal Y movement
        if (kvelocityY > kmaxVel)   kvelocityY = kmaxVel;
        if (kvelocityY < -6) kvelocityY = -6;
        kyPos += kvelocityY;
    }
    else if (passThisFrameYPos && kvelocityY > 0) {
        // Block downward movement
        kvelocityY = 0;
    }
    else if (passThisFrameYNeg && kvelocityY < 0) {
        // Block upward movement
        kvelocityY = 0;
    }
    else {
        // If blocking one side but moving the other way, allow movement
        kyPos += kvelocityY;
    }

    // --- X movement ---
    if (passThisFramePosX && kvelocityX > 0) {
        kvelocityX = 0; // stop moving right
    } 
    else if (passThisFrameNegX && kvelocityX < 0) {
        kvelocityX = 0; // stop moving left
    } 
    else {
        kxPos += kvelocityX;
    }

    // Reset forces for next frame
    kforceX = 0;
    kforceY = 0;

    positions[0] = kxPos;
    positions[1] = kyPos;

}
