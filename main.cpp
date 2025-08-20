#include <filesystem>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <tmx.h>
#include "globals.hpp"
#include "init.hpp"
#include "mapglobal.hpp"
#include "player.hpp"
#include "texture.hpp"
#include "render_loop.hpp"
#include <tinyxml2.h>
bool shouldRestart = false;

// player
Player player;
// main function
int main(int argv, char *args[])
{
    const char *pathToTMX = "levels/7.tmx";
    // initialize
    if (!init())
    {
        cout << "Error initializing subsystems!" << endl;
        return -1;
    }
    // heart of the game
    do{
	shouldRestart = false;
	enemies = {};
	tileSet = {};
        if (!renderLoop(pathToTMX))
 	   {
 	       cout << "Error running the game loop!" << endl;
	        return -1;
	    }

    }
    while(shouldRestart);
          return 0;
}
