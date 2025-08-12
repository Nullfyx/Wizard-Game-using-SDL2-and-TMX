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

// player
Player player;
// main function
int main(int argv, char *args[])
{
    const char *pathToTMX = "levels/5.tmx";
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
