#include "texture.hpp"
#include "timer.hpp"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
// constructor
int WTexture::fps = 8;
WTexture::WTexture() {
  // clear garbage values
  wTexture = NULL;
  wWidth = 0;
  wHeight = 0;
  angle = 0;
}

// remove the texture
void WTexture::free() {
  // assign null to texture
  wTexture = NULL;

  // reset dimensions
  wHeight = 0;
  wWidth = 0;
}

// image load to texture
void WTexture::WIMG_Load(string src) {
  // free any pre existing texture
  free();

  // generate a temporary surface to load image
  SDL_Surface *loadedImage = IMG_Load(src.c_str());

  // if error
  if (!loadedImage) {
    cout << "Error loading image: " << src << " SDL ERR: " << IMG_GetError()
         << endl;
    return;
  }

  // generate a temporary texture to load image
  SDL_Texture *loadedTexture =
      SDL_CreateTextureFromSurface(wRenderer, loadedImage);

  // if error
  if (!loadedTexture) {
    cout << "Error Generating Texture, SDL ERR: " << IMG_GetError() << endl;
    return;
  }

  // assign the texture
  wTexture = loadedTexture;

  // set the dimensions
  SDL_QueryTexture(loadedTexture, NULL, NULL, &this->wWidth, &this->wHeight);
  // clean
  SDL_FreeSurface(loadedImage);
}

void WTexture::setTexture(SDL_Texture *tex) {
  free();
  wTexture = tex;
}
// text to texture
void WTexture::textTex(string str, TTF_Font *wFont, SDL_Color wColor,
                       SDL_Renderer *wRenderer) {
  // if the font variable is NULL
  if (!wFont) {
    cout << "Error loading fonts" << endl;
    return;
  }

  // free preexisting font
  free();

  // write text to surface
  SDL_Surface *loadedSurface =
      TTF_RenderText_Blended(wFont, str.c_str(), wColor);

  // if error
  if (!loadedSurface) {
    cout << "Error writing text to surface, SDL ERR: " << TTF_GetError()
         << endl;
    return;
  }

  // create texture from surface
  SDL_Texture *loadedTexture =
      SDL_CreateTextureFromSurface(wRenderer, loadedSurface);

  // if error
  if (!loadedTexture) {
    cout << "Error generating texture from text: " << str
         << " SDL ERR: " << SDL_GetError() << endl;
    return;
  }

  // assign texture and dimensions
  wTexture = loadedTexture;
  SDL_QueryTexture(loadedTexture, NULL, NULL, &this->wWidth, &this->wHeight);
}

// animate the sprite
// animate the sprite
void WTexture::animateSprite(SDL_Renderer *wRenderer, int cols, int cells,
                             SDL_Rect *destRect, double angle,
                             SDL_Point *center, SDL_RendererFlip flip,
                             SDL_FRect *fRect) {
  static int frame = 0;
  static Timer animationTimer;
  static bool timerStarted = false;

  if (!timerStarted) {
    animationTimer.start();
    timerStarted = true;
  }

  Uint32 frameDelay = 1000 / fps;

  if (animationTimer.getTicks() >= frameDelay) {
    frame = (frame + 1) % (cells != 0 ? cells : 1);
    animationTimer.start(); // Reset the timer
  }

  int rows = 1;
  SDL_Rect srcRect;

  int frameWidth, frameHeight;
  if (cols != 0) {
    rows = cells / cols;

    // --- FIX ---
    // Add a guard against division by zero.
    // If 'cells' is 0 (or 'cells' < 'cols'), 'rows' will be 0.
    // We must prevent dividing by 'rows' if it's 0.
    if (rows == 0) {
      rows = 1; // Default to 1 row.
    }
    // --- END FIX ---

    frameWidth = wWidth / cols;
    frameHeight = wHeight / rows; // <-- This line was crashing

    srcRect.x = (frame % cols) * frameWidth;
    srcRect.y = (frame / cols) * frameHeight;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;
  } else {
    // --- FIX ---
    // If cols is 0, we're not animating.
    // We must initialize srcRect or SDL_RenderCopy will read garbage memory.
    // This sets srcRect to the entire texture.
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = wWidth;
    srcRect.h = wHeight;
    // --- END FIX ---
  }

  //  Automatically pick between integer or float rendering
  if (fRect != NULL) {
    fRender(wRenderer, &srcRect, fRect, angle, (SDL_FPoint *)center, flip);
  } else {
    render(wRenderer, &srcRect, destRect, angle, center, flip);
  }
}

// render the texture using float precision
void WTexture::fRender(SDL_Renderer *renderer, SDL_Rect *srcRect,
                       SDL_FRect *destRect, double angle, SDL_FPoint *center,
                       SDL_RendererFlip flip) {
  if (angle != 0.0 || center != NULL || flip != SDL_FLIP_NONE) {
    SDL_RenderCopyExF(renderer, wTexture, srcRect, destRect, angle, center,
                      flip);
  } else {
    SDL_RenderCopyF(renderer, wTexture, srcRect, destRect);
  }
}
SDL_Texture *WTexture::getTexture() { return wTexture; }
// render the texture
void WTexture::render(SDL_Renderer *renderer, SDL_Rect *srcRect,
                      SDL_Rect *destRect, double angle, SDL_Point *center,
                      SDL_RendererFlip flip) {
  if (angle != 0.0 || center != NULL ||
      flip != SDL_FLIP_NONE) { // Use extended function if rotation or flip is
                               // involved
    SDL_RenderCopyEx(renderer, wTexture, srcRect, destRect, angle, center,
                     flip);
  } else {
    // Use simple render
    SDL_RenderCopy(renderer, wTexture, srcRect, destRect);
  }
}
void WTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
  // Modulate texture
  SDL_SetTextureColorMod(wTexture, red, green, blue);
}
void WTexture::setBlendMode(SDL_BlendMode blending) {
  // Set blending function
  SDL_SetTextureBlendMode(wTexture, blending);
}

void WTexture::setAlpha(Uint8 alpha) {
  // Modulate texture alpha
  SDL_SetTextureAlphaMod(wTexture, alpha);
}
// selected setters and getters
void WTexture::setCols(int c) { wCols = c; }

void WTexture::setCells(int c) { wCells = c; }

void WTexture::setFPS(int f) { fps = f; }

int WTexture::getCols() { return wCols; }

int WTexture::getCells() { return wCells; }

int WTexture::getFPS() { return fps; }

void WTexture::readAlpha(Uint8 &a) { SDL_GetTextureAlphaMod(wTexture, &a); }
