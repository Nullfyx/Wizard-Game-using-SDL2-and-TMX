#include <iostream>
#include <SDL2/SDL.h>
using namespace std;
// to be constitutioned into objects
class Gravity
{
    // gravity constant
    float wG;

public:
    // default constructor for earth
    Gravity();
    // get the value of g
    float g();
    // set the value of g
    void setG(float g);
};