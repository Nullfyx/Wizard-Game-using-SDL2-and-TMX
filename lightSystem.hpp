#ifndef LIGHTSYSTEM_HPP
#define LIGHTSYSTEM_HPP

#include <SDL2/SDL.h>
#include <vector>

struct Light {
  float x, y;
  float radius;
  float intensity;
  Uint8 r = 0, g = 0, b = 255; // default blue wizardly glow
};

class LightSystem {
private:
  SDL_Texture *gradientTex = nullptr; // radial gradient texture
  std::vector<Light> lights;

public:
  static LightSystem *active; // global access pointer

  LightSystem(int scale = 1, float tweakFactor = 2.0f);
  ~LightSystem();

  void addLight(const Light &light);   // add a light this frame
  void clearLights();                  // clear all lights (start of frame)
  void render(SDL_Renderer *renderer); // render all lights
};

#endif // LIGHTSYSTEM_HPP
