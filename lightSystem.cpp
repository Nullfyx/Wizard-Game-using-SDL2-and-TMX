// lightSystem.cpp
#include "lightSystem.hpp"
#include "mapglobal.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

// Uncomment for debug logging
// #define LIGHT_DEBUG

LightSystem *LightSystem::active = nullptr;

static const int TEX_SIZE = 64;       // gradient texture size
static float g_darknessFactor = 0.8f; // 0 = no dark, 1 = max dark

// ----------------------------------------------------------------------------
// Gradient texture helpers
// ----------------------------------------------------------------------------
static SDL_Texture *createCircularGradient(SDL_Renderer *renderer) {
  SDL_Surface *surf =
      SDL_CreateRGBSurface(0, TEX_SIZE, TEX_SIZE, 32, 0x00FF0000, 0x0000FF00,
                           0x000000FF, 0xFF000000);
  if (!surf) {
    std::cerr << "Failed circle surface: " << SDL_GetError() << "\n";
    return nullptr;
  }

  Uint32 *pixels = static_cast<Uint32 *>(surf->pixels);
  const int center = TEX_SIZE / 2;

  for (int y = 0; y < TEX_SIZE; ++y) {
    for (int x = 0; x < TEX_SIZE; ++x) {
      float dx = x - center;
      float dy = y - center;
      float dist = std::sqrt(dx * dx + dy * dy);
      float alpha = 1.0f - (dist / center);
      if (alpha < 0.0f)
        alpha = 0.0f;
      Uint8 a = static_cast<Uint8>(alpha * 255.0f);
      pixels[y * TEX_SIZE + x] =
          (static_cast<Uint32>(a) << 24) | (255u << 16) | (255u << 8) | 255u;
    }
  }

  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
  if (!tex) {
    std::cerr << "Failed circle texture: " << SDL_GetError() << "\n";
    return nullptr;
  }
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  return tex;
}

static SDL_Texture *createRectangularGradient(SDL_Renderer *renderer) {
  SDL_Surface *surf =
      SDL_CreateRGBSurface(0, TEX_SIZE, TEX_SIZE, 32, 0x00FF0000, 0x0000FF00,
                           0x000000FF, 0xFF000000);
  if (!surf) {
    std::cerr << "Failed rect surface: " << SDL_GetError() << "\n";
    return nullptr;
  }

  Uint32 *pixels = static_cast<Uint32 *>(surf->pixels);
  const int center = TEX_SIZE / 2;

  for (int y = 0; y < TEX_SIZE; ++y) {
    float dy = std::abs(y - center) / static_cast<float>(center);
    for (int x = 0; x < TEX_SIZE; ++x) {
      float dx = std::abs(x - center) / static_cast<float>(center);
      float alpha = 1.0f - std::max(dx, dy);
      if (alpha < 0.0f)
        alpha = 0.0f;
      Uint8 a = static_cast<Uint8>(alpha * 255.0f);
      pixels[y * TEX_SIZE + x] =
          (static_cast<Uint32>(a) << 24) | (255u << 16) | (255u << 8) | 255u;
    }
  }

  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
  if (!tex) {
    std::cerr << "Failed rect texture: " << SDL_GetError() << "\n";
    return nullptr;
  }
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  return tex;
}

// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------
LightSystem::LightSystem(int scale, float tweakFactor) {
  active = this;

  // Clamp tweakFactor to [0..1] for darkness control
  if (tweakFactor <= 0.0f)
    g_darknessFactor = 0.0f;
  else if (tweakFactor >= 1.0f)
    g_darknessFactor = 1.0f;
  else
    g_darknessFactor = tweakFactor;

  extern SDL_Renderer *wRenderer; // from your engine

  gradientTexCircle = createCircularGradient(wRenderer);
  gradientTexRectangle = createRectangularGradient(wRenderer);

#ifdef LIGHT_DEBUG
  std::cout << "[LightSystem] Init with darkness=" << g_darknessFactor << "\n";
#endif
}

LightSystem::~LightSystem() {
  if (gradientTexCircle)
    SDL_DestroyTexture(gradientTexCircle);
  if (gradientTexRectangle)
    SDL_DestroyTexture(gradientTexRectangle);
  gradientTexCircle = nullptr;
  gradientTexRectangle = nullptr;

  if (active == this)
    active = nullptr;

#ifdef LIGHT_DEBUG
  std::cout << "[LightSystem] Destroyed\n";
#endif
}

// ----------------------------------------------------------------------------
// Lights
// ----------------------------------------------------------------------------
void LightSystem::addLight(const Light &light) { lights.push_back(light); }

void LightSystem::clearLights() { lights.clear(); }

// ----------------------------------------------------------------------------
// Render
// ----------------------------------------------------------------------------
void LightSystem::render(SDL_Renderer *renderer) {
  if (!renderer)
    return;

  // Step 1: Darken screen
  SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  Uint8 darknessAlpha =
      static_cast<Uint8>(std::clamp(g_darknessFactor, 0.0f, 1.0f) * 255.0f);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, darknessAlpha);
  SDL_RenderFillRect(renderer, &screenRect);

  // Step 2: Add lights
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

  for (const Light &light : lights) {
    SDL_Rect dst;

    if (light.shape == Light::CIRCLE) {
      int d = static_cast<int>(light.radius * 2.0f);
      dst = {static_cast<int>(light.x - light.radius),
             static_cast<int>(light.y - light.radius), d, d};
      if (gradientTexCircle) {
        SDL_SetTextureColorMod(gradientTexCircle, light.b, light.g, light.r);
        SDL_SetTextureAlphaMod(
            gradientTexCircle,
            static_cast<Uint8>(std::clamp(light.intensity, 0.0f, 1.0f) *
                               255.0f));
        SDL_RenderCopy(renderer, gradientTexCircle, nullptr, &dst);
      }
    } else {
      int w = static_cast<int>(light.radius * 2.0f);
      int h = (light.rectHeight > 0) ? static_cast<int>(light.rectHeight * 2.0f)
                                     : w;
      dst = {static_cast<int>(light.x - light.radius),
             static_cast<int>(light.y - h / 2), w, h};
      if (gradientTexRectangle) {
        SDL_SetTextureColorMod(gradientTexRectangle, light.b, light.g, light.r);
        SDL_SetTextureAlphaMod(
            gradientTexRectangle,
            static_cast<Uint8>(std::clamp(light.intensity, 0.0f, 1.0f) *
                               255.0f));
        SDL_RenderCopy(renderer, gradientTexRectangle, nullptr, &dst);
      }
    }

#ifdef LIGHT_DEBUG
    std::cout << "Light at (" << light.x << "," << light.y
              << ") r=" << light.radius << " intensity=" << light.intensity
              << " life=" << light.life << "\n";
#endif
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

// ----------------------------------------------------------------------------
// Update
// ----------------------------------------------------------------------------
void LightSystem::update(float dt) {
  for (auto &l : lights)
    l.life -= dt;
  lights.erase(std::remove_if(lights.begin(), lights.end(),
                              [](const Light &l) { return l.life <= 0.0f; }),
               lights.end());
}
