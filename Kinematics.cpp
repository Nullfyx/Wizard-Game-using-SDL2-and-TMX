#include "Kinematics.hpp"

Kinematics::Kinematics()
{
    kgravityConstant = 9.8;
    kmass = 60;
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
}

void Kinematics::applyForce(double fX, double fY)
{
    kforceX += fX;
    kforceY += fY;
}
void Kinematics::move()
{
    std::cout << "kmass: " << kmass
              << ", kaccelerationX: " << kaccelerationX
              << ", kaccelerationY: " << kaccelerationY
              << ", kvelocityX: " << kvelocityX
              << ", kvelocityY: " << kvelocityY
              << ", kgravityConstant: " << kgravityConstant
              << ", kfps: " << kfps
              << ", kdt: " << kdt
              << ", positions: [" << kxPos << ", " << kyPos << "]"
              << ", kforceX: " << kforceX
              << ", kforceY: " << kforceY
              << std::endl;

    kaccelerationX = (kforceX) / kmass;
    kaccelerationY = (kforceY) / kmass;

    kvelocityX += kaccelerationX * kdt;
    kvelocityY += kaccelerationY * kdt;

    // Cap velocities both directions
    if (kvelocityX > 20)
        kvelocityX = 20;
    else if (kvelocityX < -20)
        kvelocityX = -20;

    if (kvelocityY > 20)
        kvelocityY = 20;
    else if (kvelocityY < -20)
        kvelocityY = -20;

    kxPos += kvelocityX;
    kyPos += kvelocityY;

    // Reset forces for next frame
    kforceX = 0;
    kforceY = 0;

    positions[0] = static_cast<double>(kxPos);
    positions[1] = static_cast<double>(kyPos);
}
