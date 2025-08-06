#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <tmx.h>
#include "globals.hpp"
#include "init.hpp"
#include "player.hpp"
#include "texture.hpp"
#include "render_loop.hpp"
#include <tinyxml2.h>

// global variables
// SDL Window
SDL_Window *wWindow = NULL;

// SDL camera
SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

// player
Player player;

// Screen dimensions
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 700;

// SDL Renderer
SDL_Renderer *wRenderer = NULL;

// main function
int main(int argv, char *args[])
{
    const char *pathToTMX = "levels/3.tmx";
    // initialize
    if (!init())
    {
        cout << "Error initializing subsystems!" << endl;
        return -1;
    }
    // heart of the game
    if (!renderLoop(pathToTMX))
    {
        cout << "Error running the game loop!" << endl;
        return -1;
    }
    return 0;
}
