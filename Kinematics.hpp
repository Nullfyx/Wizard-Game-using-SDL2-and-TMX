#ifndef KINEMATICS_HPP
#define KINEMATICS_HPP
#include <iostream>
#include <vector>
class Kinematics
{
public:
    Kinematics();
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

    void applyForce(double fX, double fY);

    void move();
};

#endif