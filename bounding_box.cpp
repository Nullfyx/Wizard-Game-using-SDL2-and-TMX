#include"bounding_box.hpp"
#include "globals.hpp"
#include <SDL2/SDL_render.h>

bool checkCollisionB(SDL_Rect &a, SDL_Rect &b)
{
 //   SDL_SetRenderDrawColor(wRenderer, 255, 0, 0, 255);
  //  SDL_RenderDrawRect(wRenderer, &a);
   // SDL_RenderDrawRect(wRenderer, &b);
    //SDL_RenderPresent(wRenderer);
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

