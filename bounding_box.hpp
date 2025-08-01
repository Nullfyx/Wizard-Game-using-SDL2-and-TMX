#ifndef BOUNDING_BOX
#define BOUNDING_BOX
#include <iostream>
#include <SDL2/SDL.h>
using namespace std;

bool checkCollisionB(SDL_Rect &a, SDL_Rect &b)
{
    // Calculate the sides of rect A
    int leftA = a.x;
    int rightA = a.x + a.w;
    int topA = a.y;
    int bottomA = a.y + a.h;

    // Calculate the sides of rect B
    int leftB = b.x;
    int rightB = b.x + b.w;
    int topB = b.y;
    int bottomB = b.y + b.h;

    // Check if there is no overlap
    if (bottomA <= topB)
        return false; // A is above B
    if (topA >= bottomB)
        return false; // A is below B
    if (rightA <= leftB)
        return false; // A is left of B
    if (leftA >= rightB)
        return false; // A is right of B

    // If none of the above, there's a collision
    return true;
}

#endif