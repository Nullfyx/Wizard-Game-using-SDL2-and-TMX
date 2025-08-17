#include "gravity.hpp"
Gravity::Gravity()
{
    wG = 9.8;
}
// get the value of g
float Gravity::g()
{
    return wG;
}

// set the value of g
void Gravity::setG(float wG)
{
    this->wG = wG;
}
