#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP
#include "globals.hpp"
#include<iostream>
#include<SDL2/SDL.h>
#include <tmx.h>

void checkCollisionsXY(tmx_map *map,
                       bool &floorCollision, bool &leftWallCollision,
                       bool &rightWallCollision, bool &ceilingCollision, bool &overlapping,
                       SDL_Rect &playerRect);

#endif
