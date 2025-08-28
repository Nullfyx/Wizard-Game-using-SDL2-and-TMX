#include "lightSystem.hpp"
#include "mapglobal.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

LightSystem *LightSystem::active = nullptr;

LightSystem::LightSystem(int scale, float tweakFactor) {
  active = this;

  // Create a simple radial gradient texture for glow
  const int texSize = 128; // tweak as needed
  SDL_Surface *surf = SDL_CreateRGBSurface(0, texSize, texSize, 32, 0x00FF0000,
                                           0x0000FF00, 0x000000FF, 0xFF000000);
  if (!surf) {
    std::cerr << "SDL_CreateRGBSurface failed: " << SDL_GetError() << std::endl;
    return;
  }

  Uint32 *pixels = (Uint32 *)surf->pixels;
  for (int y = 0; y < texSize; ++y) {
    for (int x = 0; x < texSize; ++x) {
      float dx = x - texSize / 2;
      float dy = y - texSize / 2;
      float dist = sqrtf(dx * dx + dy * dy);
      float alpha = 1.0f - dist / (texSize / 2);
      if (alpha < 0)
        alpha = 0;
      Uint8 a = (Uint8)(alpha * 255);
      pixels[y * texSize + x] =
          (a << 24) | (255 << 16) | (255 << 8) | 255; // White glow
    }
  }

  gradientTex = SDL_CreateTextureFromSurface(wRenderer, surf);

  if (!gradientTex) {
    std::cerr << "Failed to create gradient texture: " << SDL_GetError()
              << std::endl;
  }
  SDL_SetTextureBlendMode(gradientTex, SDL_BLENDMODE_BLEND);
}

LightSystem::~LightSystem() {
  if (gradientTex)
    SDL_DestroyTexture(gradientTex);
  if (active == this)
    active = nullptr;
}

void LightSystem::addLight(const Light &light) { lights.push_back(light); }

void LightSystem::clearLights() { lights.clear(); }

void LightSystem::render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 20, 20, 50, 20);
  SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_RenderFillRect(renderer, &screen);

  // Now draw each light on top, blending glow areas
  for (auto &light : lights) {
    SDL_Rect dst{(int)(light.x - light.radius), (int)(light.y - light.radius),
                 (int)(light.radius * 2), (int)(light.radius * 2)};
    SDL_SetTextureColorMod(gradientTex, light.r, light.g, light.b);
    SDL_SetTextureAlphaMod(gradientTex, (Uint8)(light.intensity * 255));

    SDL_RenderCopy(renderer, gradientTex, nullptr, &dst);
  }
}
