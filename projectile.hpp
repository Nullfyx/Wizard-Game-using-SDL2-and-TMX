#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Kinematics.hpp"
#include "bounding_box.hpp"
#include "mapglobal.hpp"
#include "texture.hpp"
#include "timer.hpp"
#include <SDL2/SDL.h>

struct projectile {
  // Constructor
  projectile(int sx, int sy, int tx, int ty);

  // Core update and logic
  void update(float dt);
  bool canFire();

  // Visuals
  WTexture texture;
  SDL_Rect proRect{};

  // Physics and motion
  Kinematics physics;
  double angle = 0.0;

  // Timing and state
  Timer timer;
  bool destroyMe = false;

  // Projectile behavior
  float speed = 8.0f;        // Units per frame
  float maxRange = 800.0f;   // Max distance before disappearing
  Uint32 lifetime = 2000;    // Lifetime in ms
  double cooldown = 0.2;     // Seconds between shots
  int power = 2;             // Damage or energy value
  double accDt = 0.0;        // Accumulator for any delta-time usage
  float particleAccumulator = 0.0f;

  // World tracking
  float startXWorld = 0.0f;
  float startYWorld = 0.0f;

  // Static counter (optional index system)
  static int index;
};

#endif

