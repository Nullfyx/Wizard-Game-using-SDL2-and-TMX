#include "globals.hpp"
#include "init.hpp"
#include "mapglobal.hpp"
#include "player.hpp"
#include "render_loop.hpp"
#include "texture.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <filesystem>
#include <iostream>
#include <tinyxml2.h>
#include <tmx.h>
bool shouldRestart = false;

// player
Player player;
// main function
int main(int argv, char *args[]) {
  const char *pathToTMX = "levels/9.tmx";
  // initialize
  if (!init()) {
    cout << "Error initializing subsystems!" << endl;
    return -1;
  }

  // --- FIX ---
  // Load global assets ONCE, before the game loop starts.
  TTF_Font *globalFont = TTF_OpenFont("./PixelOperator.ttf", 16);
  if (!globalFont) {
    cout << "FATAL: font couldn't be loaded! " << TTF_GetError() << endl;
    return -1;
  }

  SDL_Texture *backgroundTex = IMG_LoadTexture(wRenderer, "origbig.png");
  if (!backgroundTex) {
    SDL_Log("FATAL: Failed to load background texture: %s", SDL_GetError());
    return -1;
  }
  // --- END FIX ---

  // heart of the game
  do {
    shouldRestart = false;
    enemies = {};
    tileSet = {};
    *level = 0;
    *mana = 0;

    // --- FIX ---
    // Pass the already-loaded assets into the render loop.
    if (!renderLoop(pathToTMX, globalFont, backgroundTex)) {
      cout << "Error running the game loop!" << endl;
      // We'll still quit, so we can clean up
      break;
    }

  } while (shouldRestart);

  // --- FIX ---
  // Clean up the global assets after the loop is done.
  TTF_CloseFont(globalFont);
  SDL_DestroyTexture(backgroundTex);
  // Note: You'll also need to call your 'close()' function
  // that destroys wRenderer and wWindow here.

  return 0;
}
