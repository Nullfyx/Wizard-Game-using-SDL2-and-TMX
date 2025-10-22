#include "lightSystem.hpp"
#include "particleSystem.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

ParticleSystem *ParticleSystem::active = nullptr;

ParticleSystem::ParticleSystem(SDL_Renderer *renderer, float s) : scale(s) {
  active = this;

  // create small circular texture
  SDL_Surface *surf = SDL_CreateRGBSurface(0, 8, 8, 32, 0x00FF0000, 0x0000FF00,
                                           0x000000FF, 0xFF000000);

  Uint32 *pixels = (Uint32 *)surf->pixels;
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      float dx = x - 4, dy = y - 4;
      float dist = sqrtf(dx * dx + dy * dy);
      Uint8 a = dist < 4 ? 255 - (Uint8)(dist * 64) : 0;
      pixels[y * 8 + x] = (a << 24) | 0xFFFFFF;
    }
  }

  tex = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  SDL_FreeSurface(surf);
}

ParticleSystem::~ParticleSystem() {
  if (tex)
    SDL_DestroyTexture(tex);
  if (active == this)
    active = nullptr;
}

void ParticleSystem::emit(float x, float y, int count) {
  for (int i = 0; i < count; i++) {
    Particle p;
    p.x = x;
    p.y = y;

    // random velocity (scaled by speed range)
    float angle = (rand() % 360) * M_PI / 180.0f;
    float speed = minSpeed + (rand() / (float)RAND_MAX) * (maxSpeed - minSpeed);
    p.vx = cos(angle) * speed;
    p.vy = sin(angle) * speed;

    // random lifetime
    p.life = minLife + (rand() / (float)RAND_MAX) * (maxLife - minLife);
    p.l.life = p.life;
    // random size
    p.size = minSize + (rand() / (float)RAND_MAX) * (maxSize - minSize);

    // color with variance
    auto randVar = [&](Uint8 base) {
      int v = base + (rand() % (2 * colorVariance)) - colorVariance;
      return (Uint8)std::max(0, std::min(255, v));
    };
    p.r = randVar(baseR);
    p.g = randVar(baseG);
    p.b = randVar(baseB);
    p.a = baseA;

    particles.push_back(p);
  }
}

void ParticleSystem::update(float dt) {
  if (particles.empty()) {
    return;
  }

  for (size_t i = 0; i < particles.size(); ++i) {
    auto &p = particles[i];
    p.x += p.vx * 60 * dt;
    p.y += p.vy * 60 * dt;
    p.life -= dt;

    if (p.life <= 0) {
      if (LightSystem::active) {
        p.l.life = 0;
      }
      continue;
    }

    float alphaRatio = p.life / maxLife;
    if (alphaRatio < 0.2f)
      alphaRatio = 0.2f;
    p.a = static_cast<Uint8>(255 * alphaRatio);

    // Sanity check coordinates and alpha
    if (!std::isfinite(p.x) || !std::isfinite(p.y)) {
      continue;
    }

    if (p.a == 0) {
      continue;
    }

    if (LightSystem::active) {
      p.parRect.x = static_cast<int>(roundf(p.x - camera.GetFloatPosition().x));
      p.parRect.y = static_cast<int>(roundf(p.y - camera.GetFloatPosition().y));

      p.l.x = p.parRect.x;
      p.l.y = p.parRect.y;
      p.l.radius = p.size * 2.0f;
      p.l.intensity = (float)p.a / 255;
      p.l.r = p.r;
      p.l.g = p.g;
      p.l.b = p.b;
      p.l.life = p.life;

      LightSystem::active->addLight(p.l);
    }
  }

  // Clean up dead particles
  size_t beforeCount = particles.size();
  particles.erase(std::remove_if(particles.begin(), particles.end(),
                                 [](const Particle &p) { return p.life <= 0; }),
                  particles.end());
  size_t afterCount = particles.size();
}

void ParticleSystem::render(SDL_Renderer *renderer, float zoom, float camX,
                            float camY) {
  for (auto &p : particles) {
    SDL_Rect dst{(int)((p.x - camX - p.size / 2)),
                 (int)((p.y - camY - p.size / 2)), (int)(p.size),
                 (int)(p.size)};
    SDL_SetTextureColorMod(tex, p.r, p.g, p.b);
    SDL_SetTextureAlphaMod(tex, p.a);
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
  }
}
