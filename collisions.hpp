#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP
#include "globals.hpp"
#include<iostream>
#include<SDL2/SDL.h>
#include <tmx.h>
#include <vector>

#include <cmath> // for fmod

void checkCollisionsXY(tmx_map *map,
                       bool &floorCollision, bool &leftWallCollision,
                       bool &rightWallCollision, bool &ceilingCollision, bool &overlapping,
                       SDL_Rect &playerRect);

#endif
