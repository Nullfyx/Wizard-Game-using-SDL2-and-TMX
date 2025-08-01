#ifndef RENDER_LOOP_HPP
#define RENDER_LOOP_HPP
#include <iostream>
#include <SDL2/SDL.h>
#include <tmx.h>
#include "map.hpp"
#include "globals.hpp"
#include "player.hpp" //also includes texture.hpp and timer.hpp
using namespace std;
bool renderLoop(const char *path);
#endif
