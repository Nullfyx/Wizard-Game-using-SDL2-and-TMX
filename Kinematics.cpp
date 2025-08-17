
#include "Kinematics.hpp"

Kinematics::Kinematics()
{
    kminVelX = 0;
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
    kminVelX = 0;
}

void Kinematics::applyForce(double fX, double fY)
{
    kforceX += fX;
    kforceY += fY;
}


void Kinematics::move()
{
    static double remainderX = 0.0; // persists between frames

    // --- Acceleration from forces ---
    kaccelerationX = kforceX / kmass;
    kaccelerationY = kforceY / kmass; // add gravity

    // --- Velocity update ---
    kvelocityX += kaccelerationX * kdt;
    kvelocityY += kaccelerationY * kdt;

    // --- Cap velocities ---
    if (kvelocityX > kmaxVel)  kvelocityX = kmaxVel;
    if (kvelocityX < -kmaxVel) kvelocityX = -kmaxVel;
    if (kvelocityY > kmaxVel)  kvelocityY = kmaxVel;
    if (kvelocityY < -6)       kvelocityY = -6; // upward cap

    // --- Store player velocity for external use ---
    playerVelX = kvelocityX;
    playerVelY = kvelocityY;

    // --- Y movement (preserve your original logic exactly) ---
    if (!passThisFrameYPos && !passThisFrameYNeg) {
        kyPos += kvelocityY;
    } 
    else if (passThisFrameYPos && kvelocityY > 0) {
        kvelocityY = 0; // block downward
    } 
    else if (passThisFrameYNeg && kvelocityY < 0) {
        kvelocityY = 0; // block upward
    } 
    else {
        kyPos += kvelocityY; // partially blocked, allow movement
    }

    // --- X movement with remainder system ---
    remainderX += kvelocityX;

    // Handle collisions
    if (passThisFramePosX && remainderX > 0) remainderX = 0;
    if (passThisFrameNegX && remainderX < 0) remainderX = 0;

    // Apply position if accumulated movement exceeds threshold
    if (abs(remainderX) >= 0.45) {

        kxPos += remainderX;
        remainderX = 0;
    }

    // --- Reset forces ---
    kforceX = 0;
    kforceY = 0;

    // --- Update positions array ---
    positions[0] = kxPos;
    positions[1] = kyPos;
}
