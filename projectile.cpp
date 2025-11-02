#include "collisions.hpp"
#include "globals.hpp"
#include "lightSystem.hpp"
#include "mapglobal.hpp"
#include "particleSystem.hpp"
#include "projectile.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

int projectile::index = 0;

projectile::projectile(int startX, int startY, int targetX, int targetY) {
  texture.WIMG_Load("sprites/bolt.png");
  index++;

  destroyMe = false;
  lifetime = 1800; // ms
  speed = 7.0f;
  maxRange = 900.0f;

  physics.kxPos = (float)startX;
  physics.kyPos = (float)startY;

  float dx = (float)(targetX - startX);
  float dy = (float)(targetY - startY);
  float dist = std::sqrt(dx * dx + dy * dy);
  if (dist == 0)
    dist = 0.001f;

  physics.kvelocityX = (dx / dist) * speed;
  physics.kvelocityY = (dy / dist) * speed;

  startXWorld = startX;
  startYWorld = startY;

  angle = std::atan2(-dy, dx) * 180.0f / M_PI;
  if (angle < 0)
    angle += 360.0f;

  proRect = {(int)(startX - camera.GetFloatPosition().x),
             (int)(startY - camera.GetFloatPosition().y), 16, 2};
  timer.start();
}

void projectile::update(float dt) {
  // --- MOVEMENT ---
  physics.kxPos += physics.kvelocityX;
  physics.kyPos += physics.kvelocityY;

  // --- SCREEN POSITION UPDATE ---
  proRect.x = (int)(physics.kxPos - camera.GetFloatPosition().x);
  proRect.y = (int)(physics.kyPos - camera.GetFloatPosition().y);

  // --- PARTICLE TRAIL (smoky + flickery) ---
  particleAccumulator += dt;
  if (particleAccumulator >= 0.05f) {
    ParticleSystem::active->setSpeedRange(0.2f, 1.8f);
    ParticleSystem::active->setLifeRange(0.3f, 1.0f);
    ParticleSystem::active->setSizeRange(0.1f, 0.9f);
    ParticleSystem::active->setBaseColor(255, 200, 100, 1);
    ParticleSystem::active->setColorVariance(40);

    // Emit few scattered particles for cool trail
    for (int i = 0; i < 3; ++i) {
      float px = proRect.x + (std::rand() % 8 - 4);
      float py = proRect.y + (std::rand() % 8 - 4);
      ParticleSystem::active->emit(px + camera.GetFloatPosition().x,
                                   py + camera.GetFloatPosition().y, 2);
    }
    particleAccumulator = 0.0f;
  }

  // --- LIGHT EFFECT (soft flicker, orangey hue) ---
  if (LightSystem::active) {
    Light l;
    l.x = proRect.x + proRect.w / 2.0f;
    l.y = proRect.y + proRect.h / 2.0f;
    l.radius = 38 + (std::rand() % 10 - 5); // subtle flicker
    l.intensity = 0.22f + ((std::rand() % 12) / 100.0f);
    l.r = 255;
    l.g = 160;
    l.b = 60;
    LightSystem::active->addLight(l);
  }

  // --- RENDER ---
  texture.animateSprite(wRenderer, 1, 4, &proRect, -angle);

  // --- RANGE & TIME CHECK ---
  float dx = physics.kxPos - startXWorld;
  float dy = physics.kyPos - startYWorld;
  float dist = std::sqrt(dx * dx + dy * dy);

  if (dist >= maxRange || timer.getTicks() >= lifetime)
    destroyMe = true;
}

bool projectile::canFire() {
  Uint32 elapsedMs = timer.getTicks();
  if (elapsedMs >= (Uint32)(cooldown * 1000)) {
    timer.start();
    return true;
  }
  return false;
}
