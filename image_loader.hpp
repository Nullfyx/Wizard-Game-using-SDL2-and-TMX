// image_loader.cpp
#include "globals.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <tmx.h>
extern "C" void *tmx_load_texture(const char *path);
extern "C" void tmx_free_texture(void *ptr);
