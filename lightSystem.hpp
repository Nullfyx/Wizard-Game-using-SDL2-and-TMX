// lightSystem.hpp
#ifndef LIGHTSYSTEM_HPP
#define LIGHTSYSTEM_HPP

#include <SDL2/SDL.h>
#include <vector>

struct Light {
  enum Shape { CIRCLE, RECTANGLE };

  float x, y;
  float radius;       // used as radius for circle or half width for rectangle
  float intensity;
  Uint8 r = 0, g = 0, b = 255; // default blue glow
  float life = 1.0f;
  Shape shape = CIRCLE;  // default shape is circle

  // For rectangle: radius corresponds to half width; height is set separately
  float rectHeight = 0;  // Set only for RECTANGLE shape, 0 means square
};

class LightSystem {
private:
  SDL_Texture *gradientTexCircle = nullptr;    // circular radial gradient texture
  SDL_Texture *gradientTexRectangle = nullptr; // rectangular gradient texture
  std::vector<Light> lights;

public:
  static LightSystem *active; // global access pointer

  LightSystem(int scale = 1, float tweakFactor = 2.0f);
  ~LightSystem();

  void addLight(const Light &light);
  void clearLights();
  void render(SDL_Renderer *renderer);
  void update(float dt);
};

#endif // LIGHTSYSTEM_HPP

