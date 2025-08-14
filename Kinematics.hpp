#ifndef KINEMATICS_HPP
#define KINEMATICS_HPP
#include <iostream>
#include "globals.hpp"
#include <vector>
class Kinematics
{
public:
    Kinematics();
    float kminVelX;
    double kmass;
    double kaccelerationX;
    double kaccelerationY;
    double kvelocityX;
    double kvelocityY;
    double kgravityConstant;
    double kfps;
    double kdt;
    std::vector<double> positions;
    double kxPos;
    double kyPos;
    double kforceX;
    double kforceY;
bool passThisFramePosX;
bool passThisFrameNegX;
    bool passThisFrameYPos;
    bool passThisFrameYNeg;
    void applyForce(double fX, double fY);
    double kmaxVel;
    void move();
};

#endif
