
#include "globals.hpp"
#include <SDL2/SDL_ttf.h>
#include <exception>

// Screen dimensions (define before camera uses them)
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 700;

// SDL objects
SDL_Window *wWindow = nullptr;
SDL_Renderer *wRenderer = nullptr;

// Camera start position & size
Camera camera(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

// Player globals
float playerVelX = 0.0f;
float playerVelY = 0.0f;
float playerX = 0.0f;
float playerY = 0.0f;
/// mana
float manaValue = 0.0f;
float *mana = &manaValue;

// level
int levelValue = 1;
int *level = &levelValue;
// font
TTF_Font *font = NULL;
