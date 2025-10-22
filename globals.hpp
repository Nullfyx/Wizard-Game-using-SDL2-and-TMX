
#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <SDL2/SDL.h>
#include <vector>
#include "camera.hpp"
#include "SDL2/SDL_ttf.h"
extern SDL_Window *wWindow;
extern SDL_Renderer *wRenderer;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern Camera camera;
extern float playerVelX; 
extern float playerVelY;
extern float playerX;
extern float playerY;
extern bool shouldRestart;
extern float *mana;
extern int *level;
extern TTF_Font *font;
#define ZOOM_FACTOR 4.0f
#endif
