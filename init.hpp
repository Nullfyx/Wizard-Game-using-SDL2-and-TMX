#ifndef INIT_HPP
#define INIT_HPP
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include "globals.hpp"
#include "image_loader.hpp"
using namespace std;
// initialize all required subsystems
bool init();

#endif
