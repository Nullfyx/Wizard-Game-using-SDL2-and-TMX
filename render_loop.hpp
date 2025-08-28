#ifndef RENDER_LOOP_HPP
#define RENDER_LOOP_HPP
#include "globals.hpp"
#include "lightSystem.hpp"
#include "map.hpp"
#include "mapglobal.hpp"
#include "player.hpp" //also includes texture.hpp and timer.hpp
#include <SDL2/SDL.h>
#include <iostream>
#include <tmx.h>
using namespace std;
bool renderLoop(const char *path);
#endif
