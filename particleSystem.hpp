#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP
#include "globals.hpp"
#include "lightSystem.hpp"
#include <SDL2/SDL.h>
#include <vector>

struct Particle {
  float x, y;
  float vx, vy;
  float life;
  float size;
  Light l;
  Uint8 r, g, b, a;
  SDL_Rect parRect = {
    // ((World Position - Camera Position) * Zoom Factor) -> Round -> Cast to Int
    (int)std::round( ( (float)x - camera.GetFloatPosition().x ) * ZOOM_FACTOR ), 
    (int)std::round( ( (float)y - camera.GetFloatPosition().y ) * ZOOM_FACTOR ), 
    (int)((float)size * ZOOM_FACTOR), // Dimensions need scaling too!
    (int)((float)size * ZOOM_FACTOR)
};};

class ParticleSystem {
public:
  static ParticleSystem *active;

  ParticleSystem(SDL_Renderer *renderer, float scale = 1.0f);
  ~ParticleSystem();

  void emit(float x, float y, int count);
  void update(float dt);
  void render(SDL_Renderer *renderer, float zoom, float camX, float camY);

  //  Config setters
  void setScale(float s) { scale = s; }
  void setSpeedRange(float minS, float maxS) {
    minSpeed = minS;
    maxSpeed = maxS;
  }
  void setLifeRange(float minL, float maxL) {
    minLife = minL;
    maxLife = maxL;
  }
  void setSizeRange(float minSz, float maxSz) {
    minSize = minSz;
    maxSize = maxSz;
  }
  void setBaseColor(Uint8 R, Uint8 G, Uint8 B, Uint8 A) {
    baseR = R;
    baseG = G;
    baseB = B;
    baseA = A;
  }
  void setColorVariance(int variance) { colorVariance = variance; }
  void setGravity(bool g){isGravity = g;}

private:
  std::vector<Particle> particles;
  SDL_Texture *tex;
  float scale; // zoom scale
  bool isGravity = false;
  //  Config
  float minSpeed = 0.5f, maxSpeed = 2.0f;
  float minLife = 0.5f, maxLife = 1.5f;
  float minSize = 4.0f, maxSize = 8.0f;
  Uint8 baseR = 255, baseG = 200, baseB = 50, baseA = 255;
  int colorVariance = 30;
};

#endif
