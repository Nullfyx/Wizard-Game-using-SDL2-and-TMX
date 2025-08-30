#include "lightSystem.hpp"
#include "mapglobal.hpp"
#include <SDL2/SDL_image.h>
#include <algorithm> // 🔥 needed for std::remove_if
#include <iostream>

LightSystem *LightSystem::active = nullptr;

LightSystem::LightSystem(int scale, float tweakFactor) {
  active = this;

  const int texSize = 128;
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
      pixels[y * texSize + x] = (a << 24) | (255 << 16) | (255 << 8) | 255;
    }
  }

  gradientTex = SDL_CreateTextureFromSurface(wRenderer, surf);
  SDL_FreeSurface(surf);

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

void LightSystem::addLight(const Light &light) {
  lights.push_back(light);
  std::cout << "size: " << lights.size() << std::endl;
  if (lights.size() > 1) {
    std::cout << std::endl << lights[0].life << std::endl;
  }
}

void LightSystem::clearLights() { lights.clear(); }

void LightSystem::render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 20);
  SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_RenderFillRect(renderer, &screen);

  std::cout << "Rendering " << lights.size() << " lights this frame:\n";
  for (auto &light : lights) {
    std::cout << " Light: Pos=(" << light.x << ", " << light.y
              << "), Radius=" << light.radius
              << ", Intensity=" << light.intensity << ", Life=" << light.life
              << ", Color=(R:" << (int)light.r << ",G:" << (int)light.g
              << ",B:" << (int)light.b << ")\n";

    SDL_Rect dst{static_cast<int>(light.x - light.radius),
                 static_cast<int>(light.y - light.radius),
                 static_cast<int>(light.radius * 2),
                 static_cast<int>(light.radius * 2)};
    SDL_SetTextureColorMod(gradientTex, light.r, light.g, light.b);
    SDL_SetTextureAlphaMod(gradientTex,
                           static_cast<Uint8>(light.intensity * 255));
    SDL_RenderCopy(renderer, gradientTex, nullptr, &dst);
  }
}
void LightSystem::update(float dt) {
  for (auto &l : lights) {
    l.life -= dt;
    std::cout << "life: " << l.life << std::endl;
  }
  lights.erase(std::remove_if(lights.begin(), lights.end(),
                              [](const Light &l) {
                                std::cout << "removed: " << std::endl;
                                return l.life <= 0;
                              }),
               lights.end());
}
