#include "init.hpp"
#include "map.hpp" // make sure this includes the image hook declarations
#include <string>
using namespace std;

// initialize all required subsystems
bool init() {
  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "Error initializing SDL_INIT_VIDEO, SDL ERR: " << SDL_GetError()
         << endl;
    return false;
  }

  // initialize TTF
  if (TTF_Init() == -1) {
    cout << "TTF Initialization error TTF ERR: " << TTF_GetError() << endl;
    return false;
  }

  // initialize IMG
  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    cout << "IMG Initialization error IMG ERR: " << IMG_GetError() << endl;
    return false;
  }

  // initialize window
  wWindow =
      SDL_CreateWindow("wiz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  // initialize renderer
  wRenderer = SDL_CreateRenderer(wWindow, -1, SDL_RENDERER_ACCELERATED);

  // 2x zoom:
  SDL_RenderSetScale(wRenderer, 4.0f, 4.0f);

  // if error
  if (!wWindow || !wRenderer) {
    cout << "Error initializing window or renderer: SDL ERR: " << SDL_GetError()
         << endl;
    return false;
  }

  tmx_img_load_func = tmx_load_texture;
  tmx_img_free_func = tmx_free_texture;

  return true;
}
